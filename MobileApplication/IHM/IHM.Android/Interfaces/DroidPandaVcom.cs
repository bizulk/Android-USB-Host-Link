using System;
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
    public class USBBroadCastReceiver : BroadcastReceiver
    {

        /// <summary>
        /// This string must match the Android Manifest XML file
        /// </summary>
        public static String ACTION_USB_PERMISSION = "com.cio.USB_PERMISSION";

        /// <summary>
        /// Use an event to notify that the permission completed
        /// </summary>
        public event EventHandler<bool> NotifyUsbPermissionCompleted;

        /// <summary>
        /// We complete the opening device process in the brodcast receive
        /// FIXME - don't do that, just send event and let the task that triggered the permission request complete its process
        /// </summary>
        private DroidPandaVcom _droidPandaVcom;

        public USBBroadCastReceiver(DroidPandaVcom droidPandaVcom)
        {
            _droidPandaVcom = droidPandaVcom;
        }

        /// <summary>
        /// Receive the user input permission and call the Interface method to open device
        /// </summary>
        /// <param name="context"></param>
        /// <param name="intent"></param>
        public override void OnReceive(Context context, Intent intent)
        {
            String action = intent.Action;
            if (ACTION_USB_PERMISSION.Equals(action))
            {
                UsbDevice device = (UsbDevice)intent.GetParcelableExtra(UsbManager.ExtraDevice);
                bool bPermissionGranted = false;             
                if (device != null)
                {
                    bPermissionGranted = intent.GetBooleanExtra(UsbManager.ExtraPermissionGranted, false);
                }
                EventHandler<bool> handler = NotifyUsbPermissionCompleted;
                handler(this, bPermissionGranted);
            }
        }
    }

    /// <summary>
    /// THIS IS THE USB INTERFACE WE ARE WORKING ON 
    /// It requires the nuget package USBSerialForAndroid
    /// We don't use the await/async call as they are not adapted to final application.
    /// 
    /// TODO : I manage to make wit work without the userserial package, get rid of it or create another spécific implementation that use it.
    /// Selso LIBERADO selso.liberado@ciose.fr
    /// </summary>
    public class DroidPandaVcom : IUsbManager
    {
        public const string LOG_TAG = "pandavcom";

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
        
        /// <summary>
        /// The Event declared in the interface
        /// </summary>
        public event EventHandler<bool> NotifyPermRequestCompleted;

        ////////////////////////////
        // CONFIG SECTION
        private const bool bConfUseUsbManagerOnly = true; // Select method to install interface (usbserial package, or android API only)
        public const int BULK_XFER_TOUT_MS = 1000;
        public const int iVendorId = 0x0483; // Id for the STM32Nucleo
        public const int iProductID = 0x5740; // Id for the STM32Nucleo
        // END


        // usb-to-serial stuff
        private IList<IUsbSerialDriver> _availableDrivers; // Manage all available driver from package
        private IUsbSerialDriver _currDriverIface; // Currently applied driver
        // android.hardware.usb stuff
        ContextWrapper _context; // Android activity context
        DroidDevHandle _devHandle = new DroidDevHandle(); // System file descriptor we will pass to dllCom library.
        PendingIntent _usbPermissionIntent; // The permission intent is handled in the OS Interface
        USBBroadCastReceiver _USBBroadCastReceiver; // The permission broadcast receiver for USB permission intent

        /// <summary>
        /// We initialize all Android context
        /// </summary>
        /// <param name="context"></param>
        public void Init(Object context)
        {
            _context = (ContextWrapper)context;
            _devHandle.usbManager = (UsbManager)_context.GetSystemService(Context.UsbService);
            _usbPermissionIntent = PendingIntent.GetBroadcast(_context, 0, new Intent(USBBroadCastReceiver.ACTION_USB_PERMISSION), 0);
            IntentFilter filter = new IntentFilter(USBBroadCastReceiver.ACTION_USB_PERMISSION);
            _USBBroadCastReceiver = new USBBroadCastReceiver(this);
            _USBBroadCastReceiver.NotifyUsbPermissionCompleted += _USBBroadCastReceiver_NotifyUsbPermissionCompleted;
            _context.RegisterReceiver(_USBBroadCastReceiver, filter);           
        }

        private void _USBBroadCastReceiver_NotifyUsbPermissionCompleted(object sender, bool bPermissionGranted)
        {
            if (bPermissionGranted)
            {
                OpenDevice();
                // https://docs.microsoft.com/fr-fr/dotnet/api/system.eventhandler-1?view=net-5.0
                EventHandler<bool> handler = NotifyPermRequestCompleted;
                handler(this, true);
            }
            else
            {
                Log.Debug(LOG_TAG, "permission denied for device " + _devHandle.usbdev.DeviceName);
            }
        }

        // Retreive the physically connected devices.
        public ICollection<string> getListOfConnections()
        {
            // Two ways : 
            // - Use the usbserial to retreive names
            // - Use the UsbManager (but we get system path, not pretty)
            if (bConfUseUsbManagerOnly)
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
        protected int InterfaceGetBulkEndpoints(UsbInterface usbIface, ref UsbEndpoint epIn, ref UsbEndpoint epOut)
        {
            int i;
            int ret;
            for (i = 0, ret = 0; (i < usbIface.EndpointCount) && (ret < 2); i++)
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
                    if (InterfaceGetBulkEndpoints(usbIface, ref epIn, ref epOut) == 0)
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

        public bool CheckPermStatusAsync(string szDevName)
        {
            var usbDevice = _devHandle.usbManager.DeviceList[szDevName];
            // Ask for permission to access the created device
            return _devHandle.usbManager.HasPermission(usbDevice);
        }

        public void RequestPermAsync(string szDevName)
        {
            var usbDevice = _devHandle.usbManager.DeviceList[szDevName];
            _devHandle.usbManager.RequestPermission(usbDevice, _usbPermissionIntent);
        }

        public void selectDevice(string name)
        {
            if (bConfUseUsbManagerOnly)
            {

                _devHandle.usbdev = _devHandle.usbManager.DeviceList[name];
                // Check and Ask for permission to access the created device
                if (_devHandle.usbManager.HasPermission(_devHandle.usbdev))
                {                   
                    OpenDevice();
                    // https://docs.microsoft.com/fr-fr/dotnet/api/system.eventhandler-1?view=net-5.0
                    EventHandler<bool> handler = NotifyPermRequestCompleted;
                    handler(this, true);
                }
                else
                {
                    _devHandle.usbManager.RequestPermission(_devHandle.usbdev, _usbPermissionIntent);
                    // The wapp don't wait for user input and crash when asking permission 1st time
                    // We must Wait for user input for opening connection.
                }
            }
            else
            {
                // THIS IS WIP CODE THAT USES USBSERIAL.
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


        public void OpenDevice()
        {
            // At this stage Persmission must Have been granted
            if (_devHandle.usbManager.HasPermission(_devHandle.usbdev))
            {
                // Now open the port, with  the USB Manager : we get the fd/enpoints and pass it to library, no more
                _devHandle.connection = _devHandle.usbManager.OpenDevice(_devHandle.usbdev);
                if (_devHandle.connection != null)
                {
                    if (OpenInterface(_devHandle.usbdev, _devHandle.connection, ref _devHandle.usbIface, ref _devHandle.ep_in, ref _devHandle.ep_out) == 0)
                    {
                        _devHandle.fd = _devHandle.connection.FileDescriptor;
                        Log.Debug(DroidPandaVcom.LOG_TAG, "opened device endpoint" + _devHandle.usbdev.DeviceName + "with descriptor: " + _devHandle.fd);
                    }
                    else
                    {
                        Log.Debug(DroidPandaVcom.LOG_TAG, "FAILED : open device endpoint" + _devHandle.usbdev.DeviceName);
                    }
                }
            }
            else
            {
                throw new NotSupportedException("We shall have permission to device");
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

            return new DevHandle(_devHandle.fd,
                            (int)_devHandle.ep_in.Address,
                            (int)_devHandle.ep_out.Address,
                            max_pkt_size);
        }


        /// <summary>
        /// close all created ressource (except at init). So we can creat a new connection
        /// </summary>
        /// <returns></returns>
        public int Close()
        {
            /*TODO */
            return 0;
        }

        /* TODO : put in common with the write register method */
        public int ReadRegisterFromDevice(byte uiRegister, ref byte value)
        {
            // Build the request frame using dllCom 
            proto_Frame_t frame = new proto_Frame_t();
            proto_Command_t cmd = proto_Command_t.proto_CMD_GET;
            proto_frame_data_t data = new proto_frame_data_t();
            data.req.reg = uiRegister;
            data.req.value = 0;
            byte nbBytes = protocomm.proto_makeFrame(frame, cmd, data);
            byte[] buffer = new byte[protocomm.sizeof_proto_Frame_t()];
            protocomm.protoframe_serialize(frame, buffer);

            // Create a protocole instance from frame decoding (device will not be used)
            SWIGTYPE_p_proto_Device_t protodev = protocomm.devemulslave_create();
            var protoHdle = protocomm.proto_master_create(protodev);

            // Send request and receive reply
            int ret = _devHandle.connection.BulkTransfer(_devHandle.ep_out, buffer, 0, buffer.Length, BULK_XFER_TOUT_MS);
            if (ret >= 0)
            {
                Log.Debug(DroidPandaVcom.LOG_TAG, "xfer : successfully sent nb bytes : " + ret);
                // Receive the reply frame
                // First version - take the answer with the max size possible
                // TODO : Handle timeout as done by the library and returns its error.
                // We reuse the buffer as R/W frame size are constant
                ret = _devHandle.connection.BulkTransfer(_devHandle.ep_in, buffer, 0, protocomm.sizeof_proto_Frame_t(), BULK_XFER_TOUT_MS);
                if (ret >= 0)
                {
                    ret = protocomm.proto_pushToFrame(protoHdle, buffer, (uint)ret);
                    if (ret >= 0)
                    {
                        var pcmd = protocomm.new_proto_Command_t_p();
                        switch (protocomm.proto_decodeFrame(protoHdle, pcmd, data))
                        {
                            case proto_DecodeStatus_t.proto_COMPLETED:
                                if (protocomm.proto_Command_t_p_value(pcmd) == proto_Command_t.proto_CMD_REPLY)
                                {
                                    value = data.reg_value;
                                    ret = 0;
                                }
                                else
                                {
                                    ret = -1;
                                }
                                break;
                            case proto_DecodeStatus_t.proto_REFUSED:
                                ret = -1;
                                break;
                            default:
                                break;
                        }
                        protocomm.delete_proto_Command_t_p(pcmd);
                    }
                }
                else
                {
                    Log.Debug(DroidPandaVcom.LOG_TAG, "read : failed to read nb bytes");
                }
            }
            else
            {
                Log.Debug(DroidPandaVcom.LOG_TAG, "write : failed to sent nb bytes");
            }
            return ret;
        }

        public int WriteRegisterToDevice(byte uiRegister, byte value)
        {
            proto_Frame_t frame = new proto_Frame_t();
            proto_Command_t cmd = proto_Command_t.proto_CMD_SET;
            proto_frame_data_t data = new proto_frame_data_t();
            data.req.reg = uiRegister;
            data.req.value = value;
            byte nbBytes = protocomm.proto_makeFrame(frame, cmd, data);
            byte[] buffer = new byte[protocomm.sizeof_proto_Frame_t()];
            protocomm.protoframe_serialize(frame, buffer);

            // Create a protocole instance from frame decoding (device will not be used)
            SWIGTYPE_p_proto_Device_t protodev = protocomm.devemulslave_create();
            var protoHdle = protocomm.proto_master_create(protodev);
           
            int ret = _devHandle.connection.BulkTransfer(_devHandle.ep_out, buffer, 0, buffer.Length, BULK_XFER_TOUT_MS);
            if (ret >= 0)
            {
                Log.Debug(DroidPandaVcom.LOG_TAG, "xfer : successfully sent nb bytes : " + ret);
                // Receive the reply frame
                // First version - take the answer with the max size possible
                // Remember that the protocol expect the peer to seed **at most** the max frame size.
                // We reuse the buffer as R/W frame size are constant
                ret = _devHandle.connection.BulkTransfer(_devHandle.ep_in, buffer, 0, protocomm.sizeof_proto_Frame_t(), BULK_XFER_TOUT_MS);
                if (ret >= 0)
                {
                    ret = protocomm.proto_pushToFrame(protoHdle, buffer, (uint)ret);
                    if (ret >= 0)
                    {
                        var pcmd = protocomm.new_proto_Command_t_p();
                        switch (protocomm.proto_decodeFrame(protoHdle, pcmd, data))
                        {
                            case proto_DecodeStatus_t.proto_COMPLETED:
                                ret = (protocomm.proto_Command_t_p_value(pcmd) == proto_Command_t.proto_CMD_REPLY) ? 0 : 1;
                                break;
                            case proto_DecodeStatus_t.proto_REFUSED:
                                ret = -1;
                                break;
                            default:
                                break;
                        }

                        protocomm.delete_proto_Command_t_p(pcmd);
                    }
                }
                else
                {
                    Log.Debug(DroidPandaVcom.LOG_TAG, "xfer : failed to sent bytes");
                }
            }
            return ret;
        }

        public int WriteToDevice(byte[] data)
        {
            // FIXME - do the call chuncks data accordingly to max size or not ? I don't think so
            // TODO we must check the ret for the actual data len Xfered
            int ret = _devHandle.connection.BulkTransfer(_devHandle.ep_out, data, 0, data.Length, BULK_XFER_TOUT_MS);
            if (ret >= 0)
            {
                Log.Debug(DroidPandaVcom.LOG_TAG, "xfer : successfully sent nb bytes : " + ret);
            }
            else
            {
                Log.Debug(DroidPandaVcom.LOG_TAG, "xfer : failed to sent bytes retcode : " + ret);
            }
            return (ret == data.Length) ? 0 : -1;
        }

        public int ReadFromDevice(byte[] data, int len)
        {
            if (data.Length < len)
                return -1;
            // TODO we must check the ret for the actual data len Xfered
            int ret = _devHandle.connection.BulkTransfer(_devHandle.ep_in, data, 0, len, BULK_XFER_TOUT_MS);
            if (ret >= 0)
            {
                Log.Debug(DroidPandaVcom.LOG_TAG, "xfer : successfully read nb bytes : ", +ret);
            }
            else
            {
                Log.Debug(DroidPandaVcom.LOG_TAG, "xfer : failed to read bytes retcode : " + ret);
            }
            return ret ;
        }
    }
}