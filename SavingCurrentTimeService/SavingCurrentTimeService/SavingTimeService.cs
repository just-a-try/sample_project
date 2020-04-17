using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace SavingCurrentTimeService
{
    class SavingTimeService : System.ServiceProcess.ServiceBase
    {
        static void Main()
        {
            System.ServiceProcess.ServiceBase[] ServicesToRun;
            ServicesToRun =
              new System.ServiceProcess.ServiceBase[] { new WinService() };
            System.ServiceProcess.ServiceBase.Run(ServicesToRun);
        }
        private void InitializeComponent()
        {
            this.ServiceName = "WinService";
        }
    
        protected override void OnStart(string[] args)
        {
            FILE* fp = NULL;
            SYSTEMTIME time_info;
            TIME t;
            DWORD sleep_time = 65000;

            while (1)
            {
                Sleep(sleep_time);
                GetLocalTime(&time_info);
                t.hour = time_info.wHour;
                t.minute = time_info.wMinute;

                if (!(fp = fopen("time.txt", "wb")))
                {
                    cout << "file not opened properly" << endl;
                    break;
                }

                if (fwrite(&t, 4, NMEB, fp) == EOF)
                {
                    cout << "fwrite is not excecuted successfully" << endl;
                    break;
                }

                if (fp)
                {
                    if (fclose(fp) == EOF)
                    {
                        cout << "The file is not closed successfully" << endl;
                    }
                }
            }

        }
    }
}
