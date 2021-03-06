﻿using ScreenRecoder.App.Api;
using System.Drawing;
using System.Windows.Forms;

namespace ScreenRecoder.App
{
    //正在录制的红框
    public partial class FormRectRecing : Form
    {
        public FormRectRecing()
        {
            InitializeComponent();
        }
        private void FormRectRecing_Paint(object sender, PaintEventArgs e)
        {
            e.Graphics.DrawRectangle(Pens.Red, 0, 0, Width-1, Height-1);
        }

        protected override CreateParams CreateParams {
            get {
                CreateParams cp = base.CreateParams;
                cp.ExStyle |= API.WS_EX_TOOLWINDOW;
                return cp;
            }
        }
    }
}
