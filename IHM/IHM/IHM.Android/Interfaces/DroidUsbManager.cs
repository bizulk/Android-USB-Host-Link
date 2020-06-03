using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;

using IHM;

namespace IHM.Droid.Interfaces
{
    public class DroidUsbManager : IUsbManager
    {
        public int Init()
        {
            /* TODO : Do Droid Stuff */
            // Je mets -1 exprès pour mettre en evidence le fait que l'on a pas encore fait le taff
            return -1;
        }

        public int Close()
        {
            /* TODO : Do Droid Stuff */
            return 0;
        }
    }
}