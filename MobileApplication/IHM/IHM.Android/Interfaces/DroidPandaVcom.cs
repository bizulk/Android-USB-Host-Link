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
    /// Selso LIBERADO
    /// Pour les essais on utilise l'API direct car l'async exige de revoir la conception
    /// </summary>
    class DroidPandaVcom : IUsbManager
    {
        public static String ACTION_USB_PERMISSION = "com.android.example.USB_PERMISSION";
        private UsbManager _usbManager; // Usb Manager d'Androïd
        private IList<IUsbSerialDriver> _availableDrivers; // Utilise pour l'écoute de connexion USB
        IUsbSerialDriver _currDriver; // Driver appliqué à la sélection d'un périphérique
        Object _context; // C'est l'instance d'activity Android 
        int _fd; // Le descripteur de fichier crée avec l'usbmanager à passer à la lib.
        public const int iVendorId = 0x0483;
        public const int iProductID = 0x5740;

        /// <summary>
        /// L'initialisation des membres et récupération de l'instance d'USBManager
        /// </summary>
        /// <param name="context"></param>
        public void Init(Object context)
        {
            _fd = -1;
            _context = context;
            _usbManager = (UsbManager)((ContextWrapper)context).GetSystemService(Context.UsbService);
        }

        // Récupération synchrone de la liste des périphériques connectés
        public ICollection<string> getListOfConnections()
        {
#if USE_USBSERIAL_FINDER
            // On appelle la méthode USB Sérial de façon synchrone
            // Mais ça bloque et je ne sais pas pourquoi
            return getListOfConnectionsAsync().GetAwaiter().GetResult();
#else
           // L'autre méthode est d'interrroger l'USB Manager qui liste ce qu'il a
           // A vérifier : je pense que la liste est moins parlante qu'avec l'autre méthode
            return _usbManager.DeviceList.Keys;
#endif
        }


        public async Task<ICollection<string>> getListOfConnectionsAsync()
        {

            List<string> listStrDriver = new List<string>();
            // setting a unique  driver  CDC Acm for the St Eval Board
            var table = new ProbeTable();
            table.AddProduct(iVendorId, iProductID, Java.Lang.Class.FromType(typeof(CdcAcmSerialDriver)));
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
            // setting a unique  driver  CDC Acm for the St Eval Board
            var table = new ProbeTable();
            table.AddProduct(iVendorId, iProductID, Java.Lang.Class.FromType(typeof(CdcAcmSerialDriver)));
            var prober = new UsbSerialProber(table);
            _currDriver = prober.ProbeDevice(_usbManager.DeviceList[name]);
            // Ask for permission to access the created device
            if (!_usbManager.HasPermission(_currDriver.Device))
            {
                PendingIntent pi = PendingIntent.GetBroadcast((ContextWrapper)_context, 0, new Intent(ACTION_USB_PERMISSION), 0);
                _usbManager.RequestPermission(_currDriver.Device, pi);

                /* On redemande car il se peut que ça n'est pas été donnée.
                 * _usbManager.GetAccessoryList()[0])
                 * Normalement on est sur un device donc pas de requête sur accessory
                 */
                if (! _usbManager.HasPermission(_currDriver.Device) )
                {
                    return;
                }
            }

#if true
            // Use the UsbManager to retreive the native file descriptor
            UsbDeviceConnection connection = _usbManager.OpenDevice(_currDriver.Device);
            if (connection != null)
            {
                _fd = connection.FileDescriptor;
            }
#else
            CdcAcmSerialDriver cdcDriver = new CdcAcmSerialDriver(_currDriver.Device);
            UsbDeviceConnection connection = _usbManager.OpenDevice(_currDriver.Device);
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
    /// Retourner le descripteur de fichier
    /// Il est retourné dans son état courant invalide ou pas.
    /// </summary>
    /// <returns></returns>
    public int getDeviceConnection() { return _fd; }

    /// <summary>
    /// Ferme la ressource proprement
    /// </summary>
    /// <returns></returns>
    public int Close() { /*TODO */return 0;  }
    }
}