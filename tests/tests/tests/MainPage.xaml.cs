using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;

namespace tests
{
    public partial class MainPage : ContentPage
    {
        bool isConnected = false;
        public MainPage()
        {
            InitializeComponent();
        }

        void OnButtonSendClicked(object sender, EventArgs e)
        {
            //To Indent 

        }
        void OnButtonReceiveClicked(object sender, EventArgs e)
        {
            //To Indent
        }
        void OnButtonConnectClicked(object sender, EventArgs e)
        {
            //To Indent (method to connect)
            if (isConnected == true)
            {
                connectButton.IsEnabled = false;
                receiveButton.IsEnabled = true;
                sendButton.IsEnabled = true;
                disconnectButton.IsEnabled = true;
            }
        }
        void OnButtonDisconnectClicked(object sender, EventArgs e)
        {
            //To Indent (method to disconnect)
            if (isConnected == false)
            {
                connectButton.IsEnabled = true;
                receiveButton.IsEnabled = false;
                sendButton.IsEnabled = false;
                disconnectButton.IsEnabled = false;
            }
        }
    }
}
