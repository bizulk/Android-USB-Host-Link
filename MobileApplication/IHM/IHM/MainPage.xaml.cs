using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;

namespace IHM
{
    public partial class MainPage : ContentPage
    {

        ////////////////////////////
        // CONFIG SECTION
        public enum DllDeviceType
        {
            devtype_emulslave,
            devtype_serial,
            devtype_usbdev,
            devtype_libusb,
            devtype_proxy,
        };

        /// <summary>
        /// List used to display the supported protocol devices
        /// the list is indexed by enum DllDeviceType
        /// </summary>
        private readonly IList<string> _ilist_dllDev = new List<string>
            {
                "Dev type EmulSlave",
                "Dev type serial",
                "Dev type usbdev",
                "Dev type libusb",
                "Dev type proxy"
            };
        DllDeviceType   _eConfDllDevice = DllDeviceType.devtype_libusb; // Select dll device
        bool            _bConfUseAndroidforIOaccess = false; // select dll for R/W Operation OR the android usb hardware API 
        ushort          _usConfProxyPort = 5000;
        string          _szDevName; /* USB device Name */
        ////////////////////////////
        /// Handle du dll_if
        dll_if  _dll_if;
        bool    _IsConnected = false;

        // creates a list of our entries in order to process them by iterating
        List<Entry> _lRegsEntry;
        // Liste des valeurs lues
        List<Label> _lRegsLbl;

        // Or log file content : TODO log code may be refactorized
        public List<string> _lszLogs;
        private LogFile _logfile = LogFile.Instance();
        //Nom complet du fichier (chemin + nom)
        private string _Logfilename = "";
        //Texte à afficher log
        ObservableCollection<string> _lisLogs = new ObservableCollection<string>();

        /// <summary>
        /// System USM manager interface
        /// </summary>
        IUsbManager _iusbManager;
        /// <summary>
        /// USB Proxy, used in case of a device Proxy type
        /// </summary>
        IUsbProxys _iusbProxy;

        public MainPage()
        {
            InitializeComponent();         
            _dll_if = dll_if.GetInstance;

            _lRegsEntry = new List<Entry>
            {
                userReg1,
                userReg2
            };

            _lRegsLbl = new List<Label>
            {
                peerReg1,
                peerReg2
            };

            _iusbManager = Xamarin.Forms.DependencyService.Get<IUsbManager>();
            _iusbManager.NotifyPermRequestCompleted += OnPermReqCompleted;

            // Pour le log
            _Logfilename = "FileLog.log";
            string path = _logfile.GetLocalStoragePath();
            _Logfilename = Path.Combine(path, _Logfilename);
            // fill log view with the log file contact
            LogViewInit();
            listLog.ItemsSource = _lisLogs;

            switchUseApiAndroidXfer.IsToggled = _bConfUseAndroidforIOaccess;
            PickerDllDevice.ItemsSource = new List<string>(_ilist_dllDev);
            PickerDllDevice.SelectedIndex = (int)_eConfDllDevice;
        }

        void OnButtonSendClicked(object sender, EventArgs e)
        {
            string szLog = "";
            byte regVal;
            proto_Status_t status;

            for (int i = 0; i < _lRegsEntry.Count; i++)
            {
                if (_lRegsEntry[i].Text != null && _lRegsEntry[i].Text.Length > 0)
                {
                    try
                    {
                        regVal = byte.Parse(_lRegsEntry[i].Text); // Lève une exception si la valeur n'est pas entre 0 et 255
                        if (! _bConfUseAndroidforIOaccess)
                        {
                            status = _dll_if.WriteRegister((byte)i, regVal);
                        }
                        else
                        {
                            // We fill the proto status as the Interface APi does not handle it
                            // TODO : modify the API to return same code as dll
                            if (_iusbManager.WriteRegisterToDevice((byte)i, regVal) == 0)
                            {
                                status = proto_Status_t.proto_NO_ERROR;
                            }
                            else
                            {
                                status = proto_Status_t.proto_ERR_SYS;
                            }
                        }
                        szLog = "reg" + i.ToString() + "write : " + dll_if.ProtoStatusGetString(status) + ", if success refresh values";
                        _logfile.Info(szLog, ""); // Pour le stockage dans le fichier
                        _lisLogs.Insert(0, DateTime.Now.ToString(" HH:mm ") + " " + szLog);   //Pour l'affichage en temps réelle dans la dialogue

                        // After the operation we pop any message for dllCom library and add it to our log
                        // We use the encapsulated C string struc, so much easier to use for passing data
                        // TODO : clean code : wrapp it in dll_if
                        msg_t msgLog = new msg_t();
                        while (protocomm.log_global_pop_msg(msgLog) != 0)
                        {
                            // Se how easy to access the string : just take the member
                            if (string.Compare(msgLog.szMsg, "") != 0)
                            {
                                // So we add to the dialog
                                _lisLogs.Insert(0, DateTime.Now.ToString(" HH:mm ") + " " + msgLog.szMsg);
                                // and then to the file
                                _logfile.Info(msgLog.szMsg);  // Pour le stockage dans le fichier
                            }
                        }
                    }
                    catch (Exception ex)
                    {
                        szLog = "reg" + i.ToString() + " value must be between 0 and 255, " + ex.Message;
                        _lisLogs.Insert(0, DateTime.Now.ToString(" HH:mm ") + " " + szLog);   //Pour l'affichage en temps réelle dans la dialogue
                        _logfile.Error(szLog, ""); // Pour le stockage dans le fichier
                    }
                }
                else
                {
                    szLog = "reg" + i.ToString() + "not set (empty user)";
                    _lisLogs.Insert(0, DateTime.Now.ToString(" HH:mm ") + " " + szLog);   //Pour l'affichage en temps réelle dans la dialogue
                    _logfile.Error(szLog, ""); // Pour le stockage dans le fichier
                }
            }

            
        }
        void OnButtonReceiveClicked(object sender, EventArgs e)
        {
            string szLog = "";
            byte regVal = 0;
            proto_Status_t status;

            for (int i = 0; i < _lRegsLbl.Count; i++)
            {
                if (! _bConfUseAndroidforIOaccess)
                {
                    /* FIXME : operation will always fail */
                    status = _dll_if.ReadRegister((byte)i, ref regVal);
                }
                else
                {
                    // We fill the proto status as the Interface APi does not handle it
                    // TODO : modify the API to return same code as dll
                    if (_iusbManager.ReadRegisterFromDevice((byte)i, ref regVal) == 0)
                    {
                        status = proto_Status_t.proto_NO_ERROR;
                    }
                    else
                    {
                        status = proto_Status_t.proto_ERR_SYS;
                    }
                }
                szLog = "reg" + i.ToString() + "read : " + dll_if.ProtoStatusGetString(status);
                _logfile.Info(szLog, ""); // Pour le stockage dans le fichier
                _lisLogs.Insert(0, DateTime.Now.ToString(" HH:mm ") + " " + szLog);   //Pour l'affichage en temps réelle dans la dialogue
                if (status == proto_Status_t.proto_NO_ERROR)
                {
                    _lRegsLbl[i].Text = regVal.ToString();
                }
                PopDllLogs();
            }          
        }

        /// <summary>
        /// This retreive the Log from the Dll and add them to our view
        /// TODO this could be done in a separate thread to we would not need to call this each time we interact with it
        /// And in cas of blocking code the thread can retreive the logs
        /// </summary>
        void PopDllLogs()
        {
            // After some  operation we pop any message for dllCom library and add it to our log
            // We use the encapsulated C string struct, so much easier to use for passing data
            // TODO : clean code : wrapp it in dll_if
            msg_t msgLog = new msg_t();
            while (protocomm.log_global_pop_msg(msgLog) != 0)
            {
                // Se how easy to access the string : just take the member
                if (string.Compare(msgLog.szMsg, "") != 0)
                {
                    // So we add to the dialog
                    _lisLogs.Insert(0, DateTime.Now.ToString(" HH:mm ") + " " + msgLog.szMsg);
                    // and then to the file
                    _logfile.Info(msgLog.szMsg);  // Pour le stockage dans le fichier
                }
            }
        }

        void OnButtonConnectClicked(object sender, EventArgs e)
        {
            // We display the list of device name for user selection.
            ObservableCollection<string> usbNames = new ObservableCollection<string>();
            popupView.IsVisible = true;
            // We add the emulslave as it is a pseudo device
            usbNames.Add(_ilist_dllDev[(int)DllDeviceType.devtype_emulslave]);
            ICollection<string> allNames = _iusbManager.getListOfConnections();
            foreach (string name in allNames)
            {
                usbNames.Add(name);
            }
            usbList.ItemsSource = usbNames;
        }
        void OnButtonDisconnectClicked(object sender, EventArgs e)
        {
            string msgDeco = "Disconnected";

            // Fermeture de la connexion
            _dll_if.Close();
            _IsConnected = false;
            _logfile.Info(msgDeco, ""); // Pour le stockage dans le fichier
            _lisLogs.Insert(0, DateTime.Now.ToString(" HH:mm ") + " " + msgDeco);   //Pour l'affichage en temps réelle dans la dialogue
            connectButton.IsEnabled = true;
            receiveButton.IsEnabled = false;
            sendButton.IsEnabled = false;
            disconnectButton.IsEnabled = false;
            PickerDllDevice.IsEnabled = true;
        }
        void OnButtonCancelClicked(object sender, EventArgs e) // Annuler la selection de device
        {
            popupView.IsVisible = false;
        }
        void OnButtonValidateClicked(object sender, EventArgs e) // Valider la selection de device
        {
            popupView.IsVisible = false;
            connect((string)usbList.SelectedItem);
        }

        /// <summary>
        /// Refresh the GUI state according the device connection state
        /// </summary>
        void OnDeviceconnected()
        {
            // In case we forced the selected dive (emulslave) refresh display
            PickerDllDevice.SelectedIndex = (int)_eConfDllDevice;
            // Gestion de l'affichage
            if (_IsConnected)
            {
                string msgCo = "Connected";
                _logfile.Info(msgCo, ""); // Pour le stockage dans le fichier
                _lisLogs.Insert(0, DateTime.Now.ToString(" HH:mm ") + " " + msgCo);   //Pour l'affichage en temps réelle dans la dialogue
                connectButton.IsEnabled = false;
                receiveButton.IsEnabled = true;
                sendButton.IsEnabled = true;
                disconnectButton.IsEnabled = true;
                PickerDllDevice.IsEnabled = false;

            }
            else
            {
                string msgFailCo = "Fail to connect";
                _logfile.Error(msgFailCo, ""); // Pour le stockage dans le fichier
                _lisLogs.Insert(0, DateTime.Now.ToString(" HH:mm ") + " " + msgFailCo);   //Pour l'affichage en temps réelle dans la dialogue
            }
            // FIXME - application crashes if the dll device was not created
            // In the dll : check stuff was created and return an error otherwise
            PopDllLogs();
        }

        /// <summary>
        /// Performs de the connection first step : ask for permission
        /// </summary>
        /// <param name="szUsbDevName"></param>
        void connect(string szUsbDevName)
        {
            _szDevName = szUsbDevName;
            /* We call the underlying USB connection expect for emulsave*/
            if (_szDevName == _ilist_dllDev[(int)DllDeviceType.devtype_emulslave]) 
            {
                // So if we did not selected with the emulsave type selector we force it now
                // FIXME - maybe we need to update the picker to the right value
                _eConfDllDevice = DllDeviceType.devtype_emulslave;
                SWIGTYPE_p_proto_Device_t dev = _dll_if.CreateEmulslave();
                _IsConnected = (_dll_if.Open(dev, "") == 0);
                OnDeviceconnected();
            }
            else
            {
                // We assynchronously run the permission 
                // everything else is a true USB device
                _iusbManager.RequestPermAsync(szUsbDevName);
            }
        }

        private void OnPermReqCompleted(object sender, bool bPermissionGranted)
        {
            // Permission has been granted so ask for connection
            if (bPermissionGranted == true)
            {
                SWIGTYPE_p_proto_Device_t dev;
                int ret = 0;
                switch (_eConfDllDevice)
                {
                    case DllDeviceType.devtype_emulslave:
                        break;
                    /* May be we shall just passe the device type we wish to the dll so that it creates the device it self */
                    case DllDeviceType.devtype_serial:
                        // On demande a la dll de s'initialiser sans essayer d'ouvrir un port, car on va s'en occuper
                        dev = _dll_if.CreateDevSerial();
                        _IsConnected = (0 == _dll_if.Open(dev, ""));
                        if (_IsConnected)
                        {
                            // Récupére notre FD avec l'USBManager pour l'affecter à la lib
                            ret = _dll_if.SerialSetFd(dev, _iusbManager.GetDeviceConnection());
                        };
                        break;
                    case DllDeviceType.devtype_usbdev:
                        dev = _dll_if.CreateDevUsbDev();
                        _IsConnected = (0 == _dll_if.Open(dev, ""));
                        if (_IsConnected)
                        {
                            ret = _dll_if.UsbDevSetFd(dev, _iusbManager.GetDeviceConnection());
                        }
                        break;
                    case DllDeviceType.devtype_libusb:
                        dev = _dll_if.CreateDevLibUsb();
                        ret = _dll_if.LibUsbSetFd(dev, _iusbManager.GetDeviceConnection());
                        if (ret == 0)
                        {
                            _IsConnected = (0 == _dll_if.Open(dev, _szDevName));
                        }
                        break;
                    case DllDeviceType.devtype_proxy:
                        dev = _dll_if.CreateDevProxy();
                        _iusbProxy = new UsbProxy();
                        _iusbProxy.SetIUsbManager(ref _iusbManager);
                        if (_iusbProxy.Start(_usConfProxyPort))
                        {
                            string szProxyUrl = _iusbProxy.GetListenIpAddr() + protocomm.PROXY_URL_SEP + _usConfProxyPort.ToString();
                            _IsConnected = (0 == _dll_if.Open(dev, szProxyUrl));
                            if (!_IsConnected)
                            {
                                _iusbProxy.Stop();
                            }
                        }
                        break;
                    default:
                        break;
                }
            }
            else
            {
                // OUPS ! We did not have the permission
                // Cancel connection
                string szLog = "Permission to access USB device was not granted , cancelling";
                _logfile.Info(szLog, ""); // Pour le stockage dans le fichier
                _lisLogs.Insert(0, DateTime.Now.ToString(" HH:mm ") + " " + szLog);   //Pour l'affichage en temps réelle dans la dialogue
            }

            // Update GUI state
            OnDeviceconnected();
        }

        /// <summary>
        /// This Handler will process hall GUI switch toggle Event.
        /// You must always use this handler when creating a switch, and check the sender object
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void OnToggled(object sender, ToggledEventArgs e)
        {
            // Perform an action after examining e.Value
            //sender.Equals
            if( sender.Equals(switchUseApiAndroidXfer))
            {
                _bConfUseAndroidforIOaccess = switchUseApiAndroidXfer.IsToggled;
            }     
        }

        /// <summary>
        /// This Handler will process hall GUI Picker Index Event.
        /// You must always use this handler when creating a Picker, and check the sender object
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Picker_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (sender == PickerDllDevice)
            {
                _eConfDllDevice = (DllDeviceType)PickerDllDevice.SelectedIndex;
            }
        }
        // ******************* Pour le log ***************************
        private void onClickedLog(object sender, EventArgs e)
        {
            var shareService = Xamarin.Forms.DependencyService.Get<IShareService>();

            shareService.Share(_Logfilename, "Log");
        }

        /// <summary>
        /// Update Log view from the log file content
        /// </summary>
        private void LogViewInit()
        {
            FileStream fileLog;
            _lisLogs.Clear();
            // Create file if it does not exist
            if (!File.Exists(_Logfilename))
            {
                fileLog = File.Create(_Logfilename);
                fileLog.Close();
            }
            foreach (string line in File.ReadAllLines(_Logfilename))
            {
                _lisLogs.Insert(0, line);
            }
        }

        private void onClickedresetLog(object sender, EventArgs e)
        {
            _lisLogs.Clear();
            _logfile.Clear();

            File.WriteAllText(_Logfilename, "");
        }

        // *************************************************************
    }
}
