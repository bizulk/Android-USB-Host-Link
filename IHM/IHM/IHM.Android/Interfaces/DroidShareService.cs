
using Android.App;
using Android.Content;
using IHM;
using System;
using System.IO;

public class DroidShareService : IShareService
{
    public bool Share(string filename, string title)
    {
        var shareintent = new Intent(Intent.ActionSend);

        shareintent.PutExtra(Intent.ExtraTitle, title);

        shareintent.SetType("text/plain");
        string sharebodytext = File.ReadAllText(filename);
        shareintent.PutExtra(Intent.ExtraText, sharebodytext);
       
        //Création de la fenêtre de choix
        try
        {
            var chooser = Intent.CreateChooser(shareintent, "Choisissez votre option");
            chooser.AddFlags(ActivityFlags.NewTask);
            Application.Context.StartActivity(chooser);
        }
        catch (Exception e)
        {
            Console.WriteLine("{0} Exception caught.", e);
            return false;
        }

        return true;
    }
}