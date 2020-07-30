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
    /// Implementation de l'USB Manager en utilisant USBSerialForAndroid
    /// </summary>
    class DroidPandaVcom : IUsbManager
    {
        public static String ACTION_USB_PERMISSION = "com.android.example.USB_PERMISSION";
        private UsbManager _usbManager;
        private IList<IUsbSerialDriver> _availableDrivers;
        IUsbSerialDriver _currDriver;
        Object _context;
        int _fd;
        public void Init(Object context)
        {
            _fd = -1;
            _context = context;
            _usbManager = (UsbManager)((ContextWrapper)context).GetSystemService(Context.UsbService);
        }

        public ICollection<string> getListOfConnections()
        {
            // La methode UsbSerial Bloque on sait pas pourquoi
#if USE_USBSERIAL_FINDER
            return getListOfConnectionsAsync().GetAwaiter().GetResult();
#else
            return _usbManager.DeviceList.Keys;
#endif
        }

        public async Task<ICollection<string>> getListOfConnectionsAsync()
        {

            List<string> listStrDriver = new List<string>(); 
            // setting a custom driver to the default probe table
            var table = new ProbeTable();
            // On ajouter le Panda (alias ST vcom) en tant que CdC ACM
            table.AddProduct(0x0483, 0x5740, Java.Lang.Class.FromType(typeof(CdcAcmSerialDriver)));
            var prober = new UsbSerialProber(table);

            // !!! On reste bloqué dans la fonction
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
            // setting a custom driver to the default probe table
            var table = new ProbeTable();
            // On ajouter le Panda (alias ST vcom) en tant que CdC ACM
            table.AddProduct(0x0483, 0x5740, Java.Lang.Class.FromType(typeof(CdcAcmSerialDriver)));
            var prober = new UsbSerialProber(table);


            _currDriver = prober.ProbeDevice(_usbManager.DeviceList[name]);

            if (!_usbManager.HasPermission(_currDriver.Device))
            {
                PendingIntent pi = PendingIntent.GetBroadcast((ContextWrapper)_context, 0, new Intent(ACTION_USB_PERMISSION), 0);
                _usbManager.RequestPermission(_currDriver.Device, pi);
                if (! _usbManager.HasPermission(_usbManager.GetAccessoryList()[0]))
                {
                    return;
                }
            }

            UsbDeviceConnection connection = _usbManager.OpenDevice(_currDriver.Device);
            if (connection != null)
            {
                _fd = connection.FileDescriptor;
            }

#endif
        }
        public int getDeviceConnection() { return _fd; }
        public int Close() { /*TOOD*/return 0;  }
    }
}