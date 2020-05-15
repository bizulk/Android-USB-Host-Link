using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;
namespace IHM
{
    public partial class MainPage : ContentPage
    {
        //Handle du dll_if
        dll_if m_dll_if;

        bool isConnected = false;
        bool isSending = false;
        bool isReceiving = false;
        public MainPage()
        {
            InitializeComponent();

            //On récupère l'instance de dll_if pour appelé les fonctions de la DLL
            m_dll_if = dll_if.GetInstance;
        }

        void OnButtonSendClicked(object sender, EventArgs e)
        {
            //To do : call method to send
            if (isSending == true)
            {
                log.Text += "\n"+ DateTime.Now.ToString(" HH:mm") + " Sending";
            }

            if (isSending == false)
            {
                log.Text += "\n" + DateTime.Now.ToString(" HH:mm") + " Not Sending";
            }

            // TODO Test
            // Ecriture de la valeur du registre
            //m_dll_if.WriteRegister();
        }
        void OnButtonReceiveClicked(object sender, EventArgs e)
        {
            //To do : call method to receive
            if (isReceiving == true)
            {
                log.Text += "\n" + DateTime.Now.ToString(" HH:mm") + " Receiving";
            }

            if (isReceiving == false)
            {
                log.Text += "\n" + DateTime.Now.ToString(" HH:mm") + " Not Receiving";
            }

            // TODO Test
            // Lecture de la valeur du registre
            //m_dll_if.ReadRegister();
        }
        void OnButtonConnectClicked(object sender, EventArgs e)
        {
            //To do : call method to connect

            if (isConnected == false) // On a inversé les true et false pour les tests
            {
                log.Text += "\n" + DateTime.Now.ToString(" HH:mm") + " Connected";
                connectButton.IsEnabled = false;
                receiveButton.IsEnabled = true;
                sendButton.IsEnabled = true;
                disconnectButton.IsEnabled = true;

                // Test
                // Ouverture de la connexion
                m_dll_if.Open(m_dll_if.CreateEmulslave(),"unused ;)");
            }
            if (isConnected == true)
            {
                log.Text += "\n" + DateTime.Now.ToString(" HH:mm") + " Fail to connect";
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
    }
}
