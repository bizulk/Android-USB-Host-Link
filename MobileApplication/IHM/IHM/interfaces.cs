using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;
using System.Threading.Tasks;

namespace IHM
{
    /// <summary>
    /// Structure that passes OS independant USB informations
    /// </summary>
    public struct DevHandle
    {
        public int fd; /// device file descriptor
        public int ep_in; /// endpoint address for input (device to host)
        public int ep_out; /// endpoint address for output (host to device)
        public int max_pkt_size;

        public DevHandle(int fd, int ep_in, int ep_out, int max_pkt_size)
        {
            this.fd = fd;
            this.ep_in = ep_in;
            this.ep_out = ep_out;
            this.max_pkt_size = max_pkt_size;
        }
        /// Max packet size supported
    } ;

    /// <summary>
    /// This Interface defines our USB Interface implémentation
    /// </summary>
    public interface IUsbManager
    {

        /// <summary>
        /// Some custom Initialization
        /// </summary>
        /// <returns> pourquoi pas le FD, pour l'instant je fais comme ça</returns>
        void Init(Object context);

        /// <summary>
        /// Retrieve all the USB corrently connected devices
        /// </summary>
        /// <returns> une liste du nom de chaque connexion </returns>
        ICollection<string> getListOfConnections();

        /// <summary>
        /// Same But with async method, allow to wait for user to connect the device
        /// </summary>
        /// <returns></returns>
       Task<ICollection<string>> getListOfConnectionsAsync();

        /// <summary>
        /// Select device from the the connected list. After this call the device is ready for communication
        /// </summary>
        /// <returns></returns>
        void selectDevice(string name);

        /// <summary>
        /// Returns a system File descriptor for the opened connection
        /// </summary>
        /// <returns>Devide info</returns>
        DevHandle getDeviceConnection();

        /// <summary>
        /// Closes all currently opened connection.
        /// </summary>
        /// <returns></returns>
        int Close();

        /// <summary>
        /// OS specific implementation for the device protocol com that is not using our dllComm fo I/O access
        /// Written for comparison test purpose
        /// We still the dll Com 
        /// </summary>
        /// <param name="uiRegister"></param>
        /// <param name="value"></param>
        /// <returns> operation result, 0 if ok, otherwise <0 </returns>
        int ReadRegisterFromDevice(byte uiRegister, ref byte value);

        /// <summary>
        /// OS specific implementation for the device protocol com that is not using our dllComm fo I/O access
        /// </summary>
        /// <param name="uiRegister"></param>
        /// <param name="value"></param>
        /// <returns></returns>
        int WriteRegisterToDevice(byte uiRegister, byte value);

        /// <summary>
        /// Performs a write to the connected device
        /// </summary>
        /// <param name="data"></param>
        /// <returns> 0 if completed, or -1 if error (including timeout to write them completely)</returns>
        int WriteToDevice(byte[] data);

        /// <summary>
        /// Performs a read from the connected device
        /// </summary>
        /// <param name="data"></param>
        /// <param name="len"> Max len data to read</param>
        /// <returns> the read len size,  or -1 if error (no data read) </returns>
        int ReadFromDevice(byte[] data, int len);
    }

    /// <summary>
    /// Get Storage local path
    /// </summary>
    public interface ILocalStoragePath
    {
        string GetLocalStoragePath();
    }

    /// <summary>
    /// Call the system share service
    /// </summary>
    public interface IShareService
    {
        bool Share(string filename, string title);
    }

    /// <summary>
    /// This is a Proxy Server for manager USB Io access.
    /// It handle the dllCom / devproxy protocol
    /// </summary>
    public interface IUsbProxys
    {
        /// <summary>
        /// Set the IUSbManager, it depends on it for USB transferts
        /// </summary>
        /// <param name="iusbManager"></param>
        void SetIUsbManager(ref IUsbManager iusbManager);
        bool Start(ushort usPort);
        bool Stop();
        /// <summary>
        /// Get the IP addr to pass to the peer 
        /// </summary>
        /// <returns> IP v4 string XXX.XXX.XXX.XXX </returns>
        string GetListenIpAddr();
    }
}
