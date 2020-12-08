﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Android.App;
using Android.Content;
using Android.Hardware.Usb;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;
using Android.Util;
using Hoho.Android.UsbSerial.Driver;
using Hoho.Android.UsbSerial.Util;

using IHM;

namespace IHM.Droid.Interfaces
{
    /// <summary>
    /// THIS IS THE USB INTERFACE WE ARE WORKING ON 
    /// It requires the nuget package USBSerialForAndroid
    /// We don't use the await/async call as they are not adapted to final application.
    /// 
    /// TODO : I manage to make wit work without the userserial package, get rid of it or create another spécific implementation that use it.
    /// Selso LIBERADO selso.liberado@ciose.fr
    /// </summary>
    class DroidPandaVcom : IUsbManager
    {
        /// This class will keep all android hardware usb stuff
        public class DroidDevHandle
        {
            public UsbManager usbManager; // Android Usb Manager
            public int fd; // descript file gottenfrom connection
            public UsbDevice usbdev;
            public UsbDeviceConnection connection;
            public UsbInterface usbIface;
            public UsbEndpoint ep_in;
            public UsbEndpoint ep_out;
            public int max_pkt_size; // for now we don't care 

            public DroidDevHandle()
            {
                fd = -1;
                max_pkt_size = 0;
                // everything else is set to null 
            }
        }
        public static String ACTION_USB_PERMISSION = "com.android.example.USB_PERMISSION";
        // usb-to-serial stuff
        private IList<IUsbSerialDriver> _availableDrivers; // Manage all available driver from package
        IUsbSerialDriver _currDriverIface; // Currently applied driver
        // android.hardware.usb stuff
        Object _context; // Android activity context
        DroidDevHandle _devHandle = new DroidDevHandle(); // System file descriptor we will pass to dllCom library.
        public const int iVendorId = 0x0483; // Id for the STM32Nucleo
        public const int iProductID = 0x5740; // Id for the STM32Nucleo
        private const bool bUseUsbManagerOnly = true; // Select méthod internaly
        /// <summary>
        /// We initialize all Android context
        /// </summary>
        /// <param name="context"></param>
        public void Init(Object context)
        {
            _context = context;
            _devHandle.usbManager = (UsbManager)((ContextWrapper)context).GetSystemService(Context.UsbService);
        }

        // Retreive the connected device.
        public ICollection<string> getListOfConnections()
        {
            // Two ways : 
            // - Use the usbserial to retreive names
            // - Use the UsbManager (but we get system path, not pretty)
            if (bUseUsbManagerOnly)
            {
                return _devHandle.usbManager.DeviceList.Keys;
            }
            else
            {
                // BUG : freezed code :(
                return getListOfConnectionsAsync().GetAwaiter().GetResult();
            }
        }

        /// <summary>
        /// This method is based on the USBSerial package example
        /// We add the STM32 as a CDC product.
        /// BUG : we stay blocked in the FindAllDriversAsync wen calling getListOfConnections()
        /// </summary>
        /// <returns></returns>
        public async Task<ICollection<string>> getListOfConnectionsAsync()
        {

            List<string> listStrDriver = new List<string>();
            // setting a unique  driver  CDC Acm for the St Eval Board
            var table = new ProbeTable();
            table.AddProduct(iVendorId, iProductID, Java.Lang.Class.FromType(typeof(CdcAcmSerialDriver)));
            var prober = new UsbSerialProber(table);

            _availableDrivers = await prober.FindAllDriversAsync(_devHandle.usbManager);
            if (_availableDrivers.Count == 0)
            {
                return listStrDriver;
            }

            foreach (var driver in _availableDrivers)
            {
                listStrDriver.Add(driver.ToString());
            }


            return listStrDriver;
        }

        /// <summary>
        /// Retreive bulk Enpoints from the interface
        /// </summary>
        /// <param name="usbIface"></param>
        /// <param name="epIn"></param>
        /// <param name="epOut"></param>
        /// <returns></returns>
        protected int usbInterfaceGetBulkEndpoints(UsbInterface usbIface, ref UsbEndpoint epIn, ref UsbEndpoint epOut)
        {
            int i;
            int ret;
            for (i = 0, ret = 0; (i < usbIface.EndpointCount) && (ret< 2); i++)
            {
                UsbEndpoint ep = usbIface.GetEndpoint(i);
                if (ep.Type == UsbAddressing.XferBulk)
                {
                    if (ep.Direction == UsbAddressing.In)
                    {
                        epIn = ep;
                        ret++;
                    }
                    else if (ep.Direction == UsbAddressing.Out)
                    {
                        epOut = ep;
                        ret++;
                    }
                }
            }
            if (ret == 2)
            {
                return 0;
            }
            else
            {
                return -1;
            }
        }

        /// <summary>
        /// try to retreive an endpoint with the device
        /// Is seems that  usb-serial-for-android does this job
        /// </summary>
        /// <param name="connection"> The created connection</param>
        /// <param name="epIn"> Endpoint found for the interface for device to host transmission </param>
        /// <param name="epOut"> Endpoint found for the interface for host to device transmission </param>
        /// <returns> operation result </returns>
        protected int OpenInterface(UsbDevice usbdev, UsbDeviceConnection connection, ref UsbInterface usbIface, ref UsbEndpoint epIn, ref UsbEndpoint epOut)
        {
            int ret = -1;
            int i;
            // Search and claim the first interface available
            for (i = 0; i < usbdev.InterfaceCount; i++)
            {
                usbIface = usbdev.GetInterface(i);
                if (connection.ClaimInterface(usbIface, true))
                {
                    // Now search IO endpoints
                    if (usbInterfaceGetBulkEndpoints(usbIface, ref epIn, ref epOut) == 0)
                    {
                        connection.SetInterface(usbIface);
                        return 0;
                    }
                    else
                    {
                        connection.ReleaseInterface(usbIface);
                    }
                }
            }
                return ret;
        }

        public void selectDevice(string name)
        {
            if (bUseUsbManagerOnly)
            {
                // Because we did not use the UsbSerial to probe the device we do theses steps here 
                // Steps are : 
                //  - Probe device
                //  ==> put out this UbSerial code
                // Probing a unique  driver  CDC Acm for the St Eval Board
                var table = new ProbeTable();
                table.AddProduct(iVendorId, iProductID, Java.Lang.Class.FromType(typeof(CdcAcmSerialDriver)));
                var prober = new UsbSerialProber(table);
                _currDriverIface = prober.ProbeDevice(_devHandle.usbManager.DeviceList[name]);
                _devHandle.usbdev = _currDriverIface.Device;
                // Ask for permission to access the created device
                if (!_devHandle.usbManager.HasPermission(_currDriverIface.Device))
                {
                    PendingIntent pi = PendingIntent.GetBroadcast((ContextWrapper)_context, 0, new Intent(ACTION_USB_PERMISSION), 0);
                    _devHandle.usbManager.RequestPermission(_currDriverIface.Device, pi);
                    /* We check the permission was given
                     */
                    if (!_devHandle.usbManager.HasPermission(_currDriverIface.Device))
                    {
                        // Loose !
                        Log.Debug("pandavcom", "FAILED : did not have persmission to open device" + _devHandle.usbdev.DeviceName);
                        return;
                    }
                }
                // Now open the port, two ways :
                // - Use the USB Manager : we get the fd/enpoints and pass it to library, no more
                // Use the UsbManager to retreive the native file descriptor
                _devHandle.connection = _devHandle.usbManager.OpenDevice(_devHandle.usbdev);
                if (_devHandle.connection != null)
                {
                    if (OpenInterface(_devHandle.usbdev, _devHandle.connection, ref _devHandle.usbIface, ref _devHandle.ep_in, ref _devHandle.ep_out) == 0)
                    {
                        _devHandle.fd = _devHandle.connection.FileDescriptor;
                        Log.Debug("pandavcom", "opened device endpoint" + _currDriverIface.Device.DeviceName + "with descriptor: " + _devHandle.fd);
                    }
                    else
                    {
                        Log.Debug("pandavcom", "FAILED : open device endpoint" + _currDriverIface.Device.DeviceName);
                    }

                    // Juste note the following : UsbDeviceConnection wrapps the usbfs API.
                    // connection.BulkTransfer( endpoint...).
                }
            }
            else
            {
                // TODO (mais le finder marche pas pour l'instant c'est bloqué)
                CdcAcmSerialDriver cdcDriver = new CdcAcmSerialDriver(_currDriverIface.Device);
                    UsbDeviceConnection connection = _devHandle.usbManager.OpenDevice(_currDriverIface.Device);
                    if (connection != null)
                    {
                        _devHandle.fd = connection.FileDescriptor;
                    }
                    IList<IUsbSerialPort> ports = cdcDriver.Ports;
                    if (ports.Count == 0)
                        return;
                    // L'appel ici est pour réclamer l'interface
                    ports[0].Open(connection);
                    // TODO....
            }
        }

        /// <summary>
        /// Returns current file descriptor
        /// 
        /// </summary>
        /// <returns> v </returns>
        public DevHandle getDeviceConnection()
        {
                int max_pkt_size = 0;
                // TODO place this code when retreiving info
                if ((_devHandle.ep_in == null) && (_devHandle.ep_out != null))
                {
                    max_pkt_size = _devHandle.ep_out.MaxPacketSize;
                }
                else if ((_devHandle.ep_in != null) && (_devHandle.ep_out == null))
                {
                    max_pkt_size = _devHandle.ep_in.MaxPacketSize;
                }
                else if ((_devHandle.ep_in != null) && (_devHandle.ep_out != null))
                {
                    Math.Min(_devHandle.ep_in.MaxPacketSize, _devHandle.ep_out.MaxPacketSize);
                }

                return new DevHandle( _devHandle.fd, 
                                _devHandle.ep_in.EndpointNumber, 
                                _devHandle.ep_out.EndpointNumber,
                                max_pkt_size);
        }


        /// <summary>
        /// close all created ressource (except at init). So we can creat a new connection
        /// </summary>
        /// <returns></returns>
        public int Close()
        { 
                /*TODO */return 0;
        }

        public int ReadRegisterFromDevice(byte uiRegister, ref byte value)
        {
            // Build the request frame using dllCom 
            proto_Frame_t frame = new proto_Frame_t();
            proto_Command_t cmd = proto_Command_t.proto_CMD_GET;
            proto_frame_data_t data = new proto_frame_data_t();
            data.req.value = uiRegister;
            data.reg_value = value;
            byte nbBytes = protocomm.proto_makeFrame(frame, cmd, data);
            string szFrame = protocomm.proto_frame_data_t_p_to_charp(frame);
            byte[] buffer = Encoding.ASCII.GetBytes(szFrame);
          

            int ret = _devHandle.connection.BulkTransfer(_devHandle.ep_out, buffer, 0, buffer.Length, 1000);
            if (ret >= 0)
            {
                Log.Debug("pandavcom", "test : successfully sent nb bytes : " + ret);
                // Retreive the reply frame
                // TODO
            }
            else
            {
                Log.Debug("pandavcom", "test : failed to sent nb bytes");
            }


            return -1;
        }

        public int WriteRegisterToDevice(byte uiRegister, byte value)
        {
            proto_Frame_t frame = new proto_Frame_t();
            proto_Command_t cmd = proto_Command_t.proto_CMD_SET;
            proto_frame_data_t data = new proto_frame_data_t();
            data.req.value = uiRegister;
            data.reg_value = value;
            byte nbBytes = protocomm.proto_makeFrame(frame, cmd, data);
            string szFrame = protocomm.proto_frame_data_t_p_to_charp(frame);
            byte[] buffer = Encoding.ASCII.GetBytes(szFrame);

            int ret = _devHandle.connection.BulkTransfer(_devHandle.ep_out, buffer, 0, buffer.Length, 1);
            if (ret >= 0)
            {
                // TODO
                Log.Debug("pandavcom", "test : successfully sent nb bytes : " + ret);
            }
            else
            {
                Log.Debug("pandavcom", "test : failed to sent nb bytes : ");
            }

            return 0;
        }
    }

}