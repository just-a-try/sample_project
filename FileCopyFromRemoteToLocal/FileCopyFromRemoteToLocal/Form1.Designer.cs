namespace FileCopyFromRemoteToLocal
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.textBox2 = new System.Windows.Forms.TextBox();
            this.browse_button_remotepath = new System.Windows.Forms.Button();
            this.copyfile_button = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.browse_button_localpath = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(86, 57);
            this.textBox1.Name = "textBox1";
            this.textBox1.ReadOnly = true;
            this.textBox1.Size = new System.Drawing.Size(376, 20);
            this.textBox1.TabIndex = 3;
            // 
            // textBox2
            // 
            this.textBox2.Location = new System.Drawing.Point(86, 167);
            this.textBox2.Name = "textBox2";
            this.textBox2.ReadOnly = true;
            this.textBox2.Size = new System.Drawing.Size(376, 20);
            this.textBox2.TabIndex = 4;
            // 
            // browse_button_remotepath
            // 
            this.browse_button_remotepath.Location = new System.Drawing.Point(517, 57);
            this.browse_button_remotepath.Name = "browse_button_remotepath";
            this.browse_button_remotepath.Size = new System.Drawing.Size(93, 23);
            this.browse_button_remotepath.TabIndex = 5;
            this.browse_button_remotepath.Text = "Browse";
            this.browse_button_remotepath.UseVisualStyleBackColor = true;
            this.browse_button_remotepath.Click += new System.EventHandler(this.button1_Click);
            // 
            // copyfile_button
            // 
            this.copyfile_button.Location = new System.Drawing.Point(661, 104);
            this.copyfile_button.Name = "copyfile_button";
            this.copyfile_button.Size = new System.Drawing.Size(100, 41);
            this.copyfile_button.TabIndex = 7;
            this.copyfile_button.Text = "Copy file to local path";
            this.copyfile_button.UseVisualStyleBackColor = true;
            this.copyfile_button.Click += new System.EventHandler(this.button3_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(83, 41);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(69, 13);
            this.label1.TabIndex = 8;
            this.label1.Text = "Remote Path";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(83, 151);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(58, 13);
            this.label2.TabIndex = 9;
            this.label2.Text = "Local Path";
            // 
            // browse_button_localpath
            // 
            this.browse_button_localpath.Location = new System.Drawing.Point(517, 167);
            this.browse_button_localpath.Name = "browse_button_localpath";
            this.browse_button_localpath.Size = new System.Drawing.Size(93, 23);
            this.browse_button_localpath.TabIndex = 10;
            this.browse_button_localpath.Text = "Browse";
            this.browse_button_localpath.UseVisualStyleBackColor = true;
            this.browse_button_localpath.Click += new System.EventHandler(this.button2_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(790, 234);
            this.Controls.Add(this.browse_button_localpath);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.copyfile_button);
            this.Controls.Add(this.browse_button_remotepath);
            this.Controls.Add(this.textBox2);
            this.Controls.Add(this.textBox1);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.TextBox textBox2;
        private System.Windows.Forms.Button browse_button_remotepath;
        private System.Windows.Forms.Button copyfile_button;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button browse_button_localpath;
    }
}

