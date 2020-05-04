using System;
using System.IO;
using System.Windows.Forms;

namespace FileCopyFromRemoteToLocal
{
    public partial class Form1 : Form
    {
        string source, destination, source_file_name;
        private static int count = 1;

        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            OpenFileDialog open_source_file = new OpenFileDialog();

            if (open_source_file.ShowDialog() == DialogResult.OK)
            {
                textBox1.Text = open_source_file.FileName;
                source = open_source_file.FileName;
                source_file_name = open_source_file.SafeFileName;
            }

        }

        private void button2_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog open_destination_path = new FolderBrowserDialog();

            if (open_destination_path.ShowDialog() == DialogResult.OK)
            {
                textBox2.Text = open_destination_path.SelectedPath;
                destination = open_destination_path.SelectedPath;
            }

        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (source != null && destination != null)
            {
                string destination_path = Path.Combine(destination, source_file_name);
                string new_file_name;

            FIND:
                if (File.Exists(destination_path))
                {
                    string file_name_without_extension = Path.GetFileNameWithoutExtension(destination_path);
                    string file_name_extension = Path.GetExtension(destination_path);
                    new_file_name = file_name_without_extension + "(" + count.ToString() + ")" + file_name_extension;
                    count++;
                    destination_path = Path.Combine(destination, new_file_name);
                    goto FIND;
                }

                File.Copy(source, destination_path);
                MessageBox.Show("File copied");
            }
            else
            {
                if(source == null && destination == null)
                    MessageBox.Show("Select source and destination file path to copy file");
                else if (source == null)
                     MessageBox.Show("Select source file path to copy file");
                else
                    MessageBox.Show("Select destination file path to copy file");
            }
            
        }
    }
}
