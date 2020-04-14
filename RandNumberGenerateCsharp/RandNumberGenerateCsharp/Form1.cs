using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;
// This is the code for your desktop app.
// Press Ctrl+F5 (or go to Debug > Start Without Debugging) to run your app.

namespace RandNumberGenerateCsharp
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }
        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        [DllImport("RANDOMINTEGERDLL.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int fnRandomIntegerDLL();

        private void button1_Click_1(object sender, EventArgs e)
        {
            int randNumber = fnRandomIntegerDLL();
            textBox1.Text = Convert.ToString(randNumber);
        }

    }
}
