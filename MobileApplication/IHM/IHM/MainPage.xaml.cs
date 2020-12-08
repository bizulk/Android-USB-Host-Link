﻿using System;
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
        /// Handle du dll_if
        dll_if m_dll_if;
        bool _bUseDllSerialDevice = false; // Select dll serial device or usbdev
        bool isConnected = false;

        // Nous permet d'associer nos Entry à des numéro de registre et d'iterer dessu
        List<Entry> m_lRegsEntry;
        // Liste des valeurs lues
        List<Label> m_lRegsLbl;

        // Pour le fichier de log
        public List<string> textLogList;
        LogFile logfile = LogFile.Instance();
        //Nom complet du fichier (chemin + nom)
        private string m_filename = "";
        //Texte à afficher log
        ObservableCollection<string> m_textLog = new ObservableCollection<string>();


        IUsbManager usbManager_;

        public MainPage()
        {
            InitializeComponent();
            //On récupère l'instance de dll_if pour appeler les fonctions de la DLL
            m_dll_if = dll_if.GetInstance;

            // on liste nos registres
            m_lRegsEntry = new List<Entry>
            {
                userReg1,
                userReg2
            };

            m_lRegsLbl = new List<Label>
            {
                peerReg1,
                peerReg2
            };

            usbManager_ = Xamarin.Forms.DependencyService.Get<IUsbManager>();

            // Pour le log
            m_filename = "FileLog.log";
            string path = logfile.GetLocalStoragePath();
            m_filename = Path.Combine(path, m_filename);

            fillLog();

            //On remplit la listview
            listLog.ItemsSource = m_textLog;
        }

        void OnButtonSendClicked(object sender, EventArgs e)
        {
            string msgSend = "";
            byte regVal;
            proto_Status_t status;

            for (int i = 0; i < m_lRegsEntry.Count; i++)
            {
                if (m_lRegsEntry[i].Text != null && m_lRegsEntry[i].Text.Length > 0)
                {
                    try
                    {
                        regVal = byte.Parse(m_lRegsEntry[i].Text); // Lève une exception si la valeur n'est pas entre 0 et 255
                        status = m_dll_if.WriteRegister((byte)i, regVal);
                        msgSend = "reg" + i.ToString() + "val " + regVal.ToString() + ": " + dll_if.ProtoStatusGetString(status);
                        m_textLog.Insert(0, DateTime.Now.ToString(" HH:mm ") + " " + msgSend);   //Pour l'affichage en temps réelle dans la dialogue
                        logfile.Info(msgSend, "");  // Pour le stockage dans le fichier

                        //Affichage du log de la dll
                        msgSend = "";
                        while (protocomm.log_global_pop(msgSend) != 0)
                        {
                            if( string.Compare(msgSend,"") != 0 )
                            {
                                m_textLog.Insert(0, DateTime.Now.ToString(" HH:mm ") + " " + msgSend);   //Pour l'affichage en temps réelle dans la dialogue
                                logfile.Info(msgSend, "");  // Pour le stockage dans le fichier
                            }
                        }
                            
                    }
                    catch (Exception ex)
                    {
                        msgSend = "reg" + i.ToString() + " value must be between 0 and 255";
                        m_textLog.Insert(0, DateTime.Now.ToString(" HH:mm ") + " " + msgSend);   //Pour l'affichage en temps réelle dans la dialogue
                        logfile.Error(msgSend, ""); // Pour le stockage dans le fichier
                    }
                }
                else
                {
                    msgSend = "reg" + i.ToString() + "not set (empty user)";
                    m_textLog.Insert(0, DateTime.Now.ToString(" HH:mm ") + " " + msgSend);   //Pour l'affichage en temps réelle dans la dialogue
                    logfile.Error(msgSend, ""); // Pour le stockage dans le fichier
                }
            }

            
        }
        void OnButtonReceiveClicked(object sender, EventArgs e)
        {
            string msgReceive = new string('\0', 100);
            byte regVal = 0;
            proto_Status_t status;

            for (int i = 0; i < m_lRegsLbl.Count; i++)
            {
                status = m_dll_if.ReadRegister((byte)i, ref regVal);
                msgReceive = "reg" + i.ToString() + "read : " + dll_if.ProtoStatusGetString(status);
                logfile.Info(msgReceive, ""); // Pour le stockage dans le fichier
                m_textLog.Insert(0, DateTime.Now.ToString(" HH:mm ") + " " + msgReceive);   //Pour l'affichage en temps réelle dans la dialogue
                if (status == proto_Status_t.proto_NO_ERROR)
                {
                    m_lRegsLbl[i].Text = regVal.ToString();
                }

                // After the operation we pop any message for dllCom library and add it to our log
                // We use the encapsulated C string struc, so much easier to use for passing data
                msg_t msgLog = new msg_t();              
                while ( protocomm.log_global_pop_msg(msgLog) != 0 )
                {
                    // Se how easy to access the string : just take the member
                    if (string.Compare(msgLog.szMsg, "") != 0)
                    {
                        // So we add to the dialog
                        m_textLog.Insert(0, DateTime.Now.ToString(" HH:mm ") + " " + msgLog.szMsg); 
                        // and then to the file
                        logfile.Info(msgLog.szMsg);  // Pour le stockage dans le fichier
                    }
                }
            }          
        }
        void OnButtonConnectClicked(object sender, EventArgs e)
        {
            //To do : call method to connect
            ObservableCollection<string> usbNames = new ObservableCollection<string>();
            popupView.IsVisible = true;
            usbNames.Add("EmulSlave");
            ICollection<string> allNames = usbManager_.getListOfConnections();
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
            m_dll_if.Close();
            isConnected = false;
            logfile.Info(msgDeco, ""); // Pour le stockage dans le fichier
            m_textLog.Insert(0, DateTime.Now.ToString(" HH:mm ") + " " + msgDeco);   //Pour l'affichage en temps réelle dans la dialogue
            connectButton.IsEnabled = true;
            receiveButton.IsEnabled = false;
            sendButton.IsEnabled = false;
            disconnectButton.IsEnabled = false;
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
        void connect(string name)
        {         
            if (name == "EmulSlave") 
            {
                // Test
                // Ouverture de la connexion
                isConnected = (m_dll_if.Open(m_dll_if.CreateEmulslave(), "") ==0); 
            }
            else // everything else is a device
            {
                // Call the system layer to create resource
                IUsbManager iusbManager = Xamarin.Forms.DependencyService.Get<IUsbManager>();
                iusbManager.selectDevice(name);

                if (_bUseDllSerialDevice)
                {
                    // On demande a la dll de s'initialiser sans essayer d'ouvrir un port, car on va s'en occuper
                    SWIGTYPE_p_proto_Device_t dev = m_dll_if.CreateDevSerial();
                    isConnected = (0 == m_dll_if.Open(dev, ""));
                    if (isConnected)
                    {
                        // Récupére notre FD avec l'USBManager pour l'affecter à la lib
                        int ret = m_dll_if.SerialSetFd(dev, iusbManager.getDeviceConnection());
                    };
                }
                else
                {
                    SWIGTYPE_p_proto_Device_t dev = m_dll_if.CreateDevUsbDev();
                    isConnected = (0 == m_dll_if.Open(dev, ""));
                    if (isConnected)
                    {
                        int ret = m_dll_if.UsbDevSetFd(dev, iusbManager.getDeviceConnection());
                    }
                }

            }

            // Gestion de l'affichage
            if (isConnected) 
            {
                string msgCo = "Connected";
                logfile.Info(msgCo, ""); // Pour le stockage dans le fichier
                m_textLog.Insert(0, DateTime.Now.ToString(" HH:mm ") + " " + msgCo);   //Pour l'affichage en temps réelle dans la dialogue
                connectButton.IsEnabled = false;
                receiveButton.IsEnabled = true;
                sendButton.IsEnabled = true;
                disconnectButton.IsEnabled = true;
                
            }
            if (isConnected == false)
            {
                string msgFailCo = "Fail to connect";
                logfile.Error(msgFailCo, ""); // Pour le stockage dans le fichier
                m_textLog.Insert(0, DateTime.Now.ToString(" HH:mm ") + " " + msgFailCo);   //Pour l'affichage en temps réelle dans la dialogue
            }
        }

        // ******************* Pour le log ***************************
        private void onClickedLog(object sender, EventArgs e)
        {
            var shareService = Xamarin.Forms.DependencyService.Get<IShareService>();

            shareService.Share(m_filename, "Log");
        }

        private void fillLog()
        {
            FileStream fileLog;
            m_textLog.Clear();

            //Création du fichier
            if (!File.Exists(m_filename))
            {
                fileLog = File.Create(m_filename);
                fileLog.Close();
            }

            foreach (string line in File.ReadAllLines(m_filename))
            {
                m_textLog.Insert(0, line);
            }
        }

        private void onClickedresetLog(object sender, EventArgs e)
        {
            m_textLog.Clear();
            logfile.Clear();

            File.WriteAllText(m_filename, "");
        }
        // *************************************************************
    }
}