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
        public MainPage()
        {
            InitializeComponent();
        }

        /*void OnButtonClicked(object sender, EventArgs e)
        {
            (sender as Button).Text = "Sah quel plaisir";
        }*/
        void OnEntry(object sender, TextChangedEventArgs e)
        {
            string old = e.OldTextValue;
            string newt = e.NewTextValue; 
        }
        void Completed(object sender, EventArgs e)
        {
            string text = ((Entry)sender).Text;
        }
    }
}
