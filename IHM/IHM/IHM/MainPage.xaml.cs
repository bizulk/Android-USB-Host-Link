using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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

        bool isConnected = false;

        // Nous permet d'associer nos Entry à des numéro de registre et d'iterer dessu
        List<Entry> m_lRegsEntry;
        // Liste des valeurs lues
        List<Label> m_lRegsLbl;

        IUsbManager usbManager_;

        public MainPage()
        {
            InitializeComponent();
        }

        public MainPage(IUsbManager usbManager)
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

            usbManager_ = usbManager;
        }

        void OnButtonSendClicked(object sender, EventArgs e)
        {
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
                        log.Text += "\n" + DateTime.Now.ToString(" HH:mm ") + "reg" + i.ToString() + "val " + regVal.ToString() + ": " + dll_if.ProtoStatusGetString(status);
                    }
                    catch (Exception ex)
                    {
                        log.Text += "\n" + DateTime.Now.ToString(" HH:mm ") + "reg" + i.ToString() + " value must be between 0 and 255";
                    }
                }
                else
                {
                    log.Text += "\n" + DateTime.Now.ToString(" HH:mm ") + "reg" + i.ToString() + "not set (empty user)";
                }
            }
        }
        void OnButtonReceiveClicked(object sender, EventArgs e)
        {
            byte regVal = 0;
            proto_Status_t status;

            for (int i = 0; i < m_lRegsLbl.Count; i++)
            {
                status = m_dll_if.ReadRegister((byte)i, ref regVal);
                log.Text += "\n" + DateTime.Now.ToString(" HH:mm ") + "reg" + i.ToString() + "read : " + dll_if.ProtoStatusGetString(status);
                if (status == proto_Status_t.proto_NO_ERROR)
                {
                    m_lRegsLbl[i].Text = regVal.ToString();
                }
            }
        }
        void OnButtonConnectClicked(object sender, EventArgs e)
        {
            //To do : call method to connect
            ObservableCollection<string> usbNames = new ObservableCollection<string>();
            popupView.IsVisible = true;
            usbList.ItemsSource = usbNames;
            usbNames.Add("EmulSlave");
            usbNames.Add("Usb 1");
            usbNames.Add("Usb 2");
            usbNames.Add("Usb 3");
            usbNames.Add("Usb 4");
            usbNames.Add("Usb 5");
            usbNames.Add("Usb 6");
            usbNames.Add("Usb 7");
            usbNames.Add("Usb 8");
            usbNames.Add("Usb 9");
            ICollection<string> allNames = usbManager_.getListOfConnections();
            foreach (string name in allNames)
            {
                usbNames.Add(name);
            }
        }
        void OnButtonDisconnectClicked(object sender, EventArgs e)
        {
            //To do : call method to disconnect

            if (isConnected == false)
            {
                log.Text += "\n" + DateTime.Now.ToString(" HH:mm") + " Disconnected";
                connectButton.IsEnabled = true;
                receiveButton.IsEnabled = false;
                sendButton.IsEnabled = false;
                disconnectButton.IsEnabled = false;

                // Test
                // Fermeture de la connexion
                m_dll_if.Close();
            }
            if (isConnected == true)
            {
                log.Text += "\n" + DateTime.Now.ToString(" HH:mm") + " Fail to disconnect";
            }
        }
        void OnButtonCancelClicked(object sender, EventArgs e)
        {
            popupView.IsVisible = false;
        }
        void OnButtonValidateClicked(object sender, EventArgs e)
        {
            popupView.IsVisible = false;
            connect("EmulSlave");
        }
        void connect(string name)
        {
            if (isConnected == false) // On a inversé les true et false pour les tests
            {
                log.Text += "\n" + DateTime.Now.ToString(" HH:mm") + " Connected";
                connectButton.IsEnabled = false;
                receiveButton.IsEnabled = true;
                sendButton.IsEnabled = true;
                disconnectButton.IsEnabled = true;

                // Test
                // Ouverture de la connexion
                m_dll_if.Open(m_dll_if.CreateEmulslave(), "unused ;)");
            }
            if (isConnected == true)
            {
                log.Text += "\n" + DateTime.Now.ToString(" HH:mm") + " Fail to connect";
            }
        }
    }
}
