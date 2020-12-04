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
    /// Selso LIBERADO selso.liberado@ciose.fr
    /// </summary>
    class DroidPandaVcom : IUsbManager
    {
        public static String ACTION_USB_PERMISSION = "com.android.example.USB_PERMISSION";
        private UsbManager _usbManager; // Android Usb Manager
        private IList<IUsbSerialDriver> _availableDrivers; // Manage all available driver from package
        IUsbSerialDriver _currDriverIface; // Currently applied driver
        Object _context; // Android activity context
        int _fd; // System file descriptor we will pass to dllCom library.
        public const int iVendorId = 0x0483; // Id for the STM32Nucleo
        public const int iProductID = 0x5740; // Id for the STM32Nucleo

        /// <summary>
        /// We initialize all Android context
        /// </summary>
        /// <param name="context"></param>
        public void Init(Object context)
        {
            _fd = -1;
            _context = context;
            _usbManager = (UsbManager)((ContextWrapper)context).GetSystemService(Context.UsbService);
        }

        // Retreive the connected device.
        public ICollection<string> getListOfConnections()
        {
            // Two ways : 
            // - Use the usbserial to retreive names
            // - Use the UsbManager (but we get system path, not pretty)
#if USE_USBSERIAL_FINDER
            // BUG : freezed code :(
            return getListOfConnectionsAsync().GetAwaiter().GetResult();
#else
            return _usbManager.DeviceList.Keys;
#endif
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

            _availableDrivers = await prober.FindAllDriversAsync(_usbManager);
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
    

   
        public void selectDevice(string name)
        {
#if USE_USBSERIAL_FINDER
            // TODO (mais le finder marche pas pour l'instant c'est bloqué)
#else
            // Because we did not use the UsbSerial to probe the device we do theses steps here 
            // Steps are : 
            //  - Probe device
            //  
            // Probing a unique  driver  CDC Acm for the St Eval Board
            var table = new ProbeTable();
            table.AddProduct(iVendorId, iProductID, Java.Lang.Class.FromType(typeof(CdcAcmSerialDriver)));
            var prober = new UsbSerialProber(table);
            _currDriverIface = prober.ProbeDevice(_usbManager.DeviceList[name]);
            // Ask for permission to access the created device
            if (!_usbManager.HasPermission(_currDriverIface.Device))
            {
                PendingIntent pi = PendingIntent.GetBroadcast((ContextWrapper)_context, 0, new Intent(ACTION_USB_PERMISSION), 0);
                _usbManager.RequestPermission(_currDriverIface.Device, pi);

                /* We check the permission was given
                 */
                if (! _usbManager.HasPermission(_currDriverIface.Device) )
                {
                    return;
                }
            }
            // Now open the port, two ways :
            // - Use the USB Manager : we get the FD and pass to library, no more
            // - Use USbSerial : We can do more things like R/W operations
#if true
            // Use the UsbManager to retreive the native file descriptor
            UsbDeviceConnection connection = _usbManager.OpenDevice(_currDriverIface.Device);
            if (connection != null)
            {
                _fd = connection.FileDescriptor;
                // The FD has a valid value, so I thought it should work :(
            }
#else
            CdcAcmSerialDriver cdcDriver = new CdcAcmSerialDriver(_currDriverIface.Device);
            UsbDeviceConnection connection = _usbManager.OpenDevice(_currDriverIface.Device);
            if (connection != null)
            {
                _fd = connection.FileDescriptor;
            }
            IList<IUsbSerialPort>  ports = cdcDriver.Ports;
            if (ports.Count == 0)
                return;
            // L'appel ici est pour réclamer l'interface
            ports[0].Open(connection);
#endif
#endif
        }

    /// <summary>
    /// Returns current file descriptor
    /// 
    /// </summary>
    /// <returns> v </returns>
    public int getDeviceConnection() { return _fd; }

    /// <summary>
    /// close all created ressource (except at init). So we can creat a new connection
    /// </summary>
    /// <returns></returns>
    public int Close() { /*TODO */return 0;  }
    }
}