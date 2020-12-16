using System;
using System.Collections.Generic;
using System.Text;

using System.IO;
using System.Net;
using System.Net.Sockets;

using System.Threading;
using System.Threading.Tasks;

namespace IHM
{
    /// <summary>
    /// Usb proxy implementation : a TCP server with sync comm in a thread background
    /// https://docs.microsoft.com/fr-fr/dotnet/api/system.net.sockets.tcplistener?view=net-5.0
    /// </summary>
    class UsbProxy : IUsbProxys
    {
        private IUsbManager _iusbManager;
        private const string _szIpAddr = "127.0.0.1";
        TcpListener server = null;
        private bool _bRunTask = false;
        System.Threading.Tasks.Task _srvTskHdle = null;

        public UsbProxy()
        {

        }

        ~UsbProxy()
        {
            Stop();
        }

        /// <summary>
        /// Dependency set
        /// </summary>
        /// <param name="iusbManager"></param>
        public void SetIUsbManager(ref IUsbManager iusbManager)
        {
            _iusbManager = iusbManager;
        }

        /// <summary>
        /// Launch a thread that will handle the connection
        /// Listen on 127.0.0.1
        /// https://docs.microsoft.com/fr-fr/dotnet/api/system.threading.tasks.task.run?view=net-5.0
        /// </summary>
        /// <param name="usPort"> Defined the port to listen on</param>
        /// <returns></returns>
        public bool Start(ushort usPort)
        {
            IPAddress localAddr = IPAddress.Parse(_szIpAddr);
            // TcpListener server = new TcpListener(port);
            server = new TcpListener(localAddr, usPort);
            // Start listening for client requests.
            server.Start();
            _bRunTask = true;
            _srvTskHdle = Task.Run(() => Mainloop() );
            if( ( _srvTskHdle.Status == TaskStatus.Canceled) || (_srvTskHdle.Status == TaskStatus.Faulted) )
            {
                return false;
            }
            return true;
        }

        public bool Stop()
        {
            _bRunTask = false;
            _srvTskHdle.Wait();
            return true;
        }

        private void Mainloop()
        {

            // Accept connection (blocking)
            TcpClient client = server.AcceptTcpClient();
            // Get a stream object for reading and writing
            NetworkStream stream = client.GetStream();
            int ret = 0;

            byte[] arrHeaderReq = new byte[protocomm.sizeof_devproxy_header_t()];
            devproxy_header_t headerReq = protocomm.devproxy_cast(arrHeaderReq);
            byte[] arrHeaderReply = new byte[protocomm.sizeof_devproxy_header_t()];
            devproxy_header_t headerReply = protocomm.devproxy_cast(arrHeaderReply);
            headerReply.SOF = (uint)protocomm.DEVPROXY_HEADER_MAGIC;

            while ( _bRunTask )
            {
                // Wait for a header packet 
                ret = stream.Read(arrHeaderReq, 0, arrHeaderReq.Length);
                // Decode and execute
                if( (ret < protocomm.sizeof_devproxy_header_t()) || (!IsHeaderValid(ref headerReq)) )
                {
                    headerReply.code = devproxy_opcode_t.PROXY_REP_NACK;
                    headerReply.datalen = 0;
                    stream.Write(arrHeaderReply, 0, arrHeaderReply.Length);
                }
                else
                {
                    // TODO : add try catch
                    Byte[] data = new Byte[headerReq.datalen];
                    headerReply.datalen = 0;
                    // Execute the peer CMD
                    switch (headerReq.code)
                    {
                        case devproxy_opcode_t.PROXY_CMD_READ:
                            ret = _iusbManager.ReadFromDevice(data, data.Length);
                            if (ret == 0)
                            {
                                headerReply.code = devproxy_opcode_t.PROXY_REP_DONE;
                                headerReply.datalen = (uint)data.Length;
                            }
                            else
                            {
                                headerReply.code = devproxy_opcode_t.PROXY_REP_ERR;
                            }
                            break;
                        case devproxy_opcode_t.PROXY_CMD_WRITE:
                            // read the data to pass to device
                            ret = stream.Read(data, 0, arrHeaderReq.Length);
                            if (ret == arrHeaderReq.Length)
                            {
                                // now pass it to the device
                                ret = _iusbManager.WriteToDevice(data);
                                if (ret == 0)
                                {
                                    headerReply.code = devproxy_opcode_t.PROXY_REP_DONE;
                                }
                                else
                                {
                                    headerReply.code = devproxy_opcode_t.PROXY_REP_ERR;
                                }
                            }
                            else
                            {
                                // Peer has not sent all expected data : reply NACK
                                // We Expect there is no risk to lose sync here
                                headerReply.code = devproxy_opcode_t.PROXY_REP_NACK;
                            }
                            break;
                        default:
                            // Shall not fall here
                            break;
                    }
                    // Return execution to peer
                    stream.Write(arrHeaderReply, 0, arrHeaderReply.Length);
                    // in case of a read we must also write data
                    if (headerReq.code == devproxy_opcode_t.PROXY_CMD_READ)
                    {
                        stream.Write(data, 0, data.Length);
                    }
                }      
            }
            // exiting 
            // Shutdown and end connection
            client.Close();
        }

        private bool IsHeaderValid(ref devproxy_header_t header)
        {
            bool ret = true;
            if (header.SOF != protocomm.DEVPROXY_HEADER_MAGIC)
            {
                return false;
            }
            switch (header.code)
            {
                case devproxy_opcode_t.PROXY_CMD_READ:                   
                case devproxy_opcode_t.PROXY_CMD_WRITE:
                    break;
                default:
                    ret =  false;
                    break;
            }
            ret = header.datalen > 0;

            return ret;
        }
    }
}
