using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration.Install;
using System.Linq;
using System.Reflection;
using System.Threading.Tasks;

namespace InstallCompenent
{
    [RunInstaller(true)]
    public partial class Installer1 : System.Configuration.Install.Installer
    {
        public Installer1()
        {
            InitializeComponent();
        }

        protected override void OnAfterInstall(IDictionary savedState)
        {
            Assembly assembly = Assembly.GetExecutingAssembly();
            string path = assembly.Location + "AiTools.exe";
            System.Diagnostics.Process.Start(path);
            base.OnAfterInstall(savedState);
        }
    }
}