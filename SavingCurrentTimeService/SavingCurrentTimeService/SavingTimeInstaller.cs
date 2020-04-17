using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SavingCurrentTimeService
{
    class SavingTimeInstaller
    {
        [System.ComponentModel.RunInstaller(true)]
        public class ProjectInstaller : System.Configuration.Install.Installer
        {
            private System.ServiceProcess.ServiceInstaller serviceInstaller;
            private System.ServiceProcess.ServiceProcessInstaller
                    serviceProcessInstaller;

            public ProjectInstaller()
            {
                InitializeComponent();
            }

            private void InitializeComponent()
            {
                this.serviceInstaller = new System.ServiceProcess.ServiceInstaller();
                this.serviceProcessInstaller =
                new System.ServiceProcess.ServiceProcessInstaller();
             
                this.serviceInstaller.Description = "My Windows Service description";
                this.serviceInstaller.DisplayName = "My WinService";
                this.serviceInstaller.ServiceName = "WinService";
               
                this.serviceProcessInstaller.Account =
                System.ServiceProcess.ServiceAccount.LocalService;
                this.serviceProcessInstaller.Password = null;
                this.serviceProcessInstaller.Username = null;
             
                this.Installers.AddRange(new System.Configuration.Install.Installer[] {
                this.serviceProcessInstaller,
                this.serviceInstaller});
            }
        }
    }
}
