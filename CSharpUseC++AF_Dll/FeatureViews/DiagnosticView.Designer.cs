using System.Drawing;
using System.Windows.Forms;
namespace CSharpUseC__AF_Dll
{
    partial class DiagnosticView
    {
        private System.ComponentModel.IContainer components = null;
        internal Button CaptureDiagnostic;
        internal Button Save;
        internal Button Open;
        internal RichTextBox DiagnosticText;
        private FlowLayoutPanel Tools;
        private Label Hint;
        protected override void Dispose(bool disposing) { if (disposing && components != null) components.Dispose(); base.Dispose(disposing); }
        private void InitializeComponent()
        {
            components = new System.ComponentModel.Container();
            Tools = new FlowLayoutPanel(); CaptureDiagnostic = new Button(); Save = new Button(); Open = new Button(); Hint = new Label(); DiagnosticText = new RichTextBox();
            Tools.SuspendLayout(); SuspendLayout();
            Tools.AutoSize = true; Tools.BackColor = Color.White; Tools.Dock = DockStyle.Top; Tools.Padding = new Padding(4);
            Tools.Controls.Add(CaptureDiagnostic); Tools.Controls.Add(Save); Tools.Controls.Add(Open); Tools.Controls.Add(Hint);
            CaptureDiagnostic.AutoSize = true; CaptureDiagnostic.BackColor = Color.FromArgb(25, 118, 210); CaptureDiagnostic.FlatStyle = FlatStyle.Flat; CaptureDiagnostic.ForeColor = Color.White; CaptureDiagnostic.MinimumSize = new Size(110, 30); CaptureDiagnostic.Text = "采集当前诊断";
            Save.AutoSize = true; Save.BackColor = Color.White; Save.FlatStyle = FlatStyle.Flat; Save.ForeColor = Color.FromArgb(26,53,73); Save.MinimumSize = new Size(86, 30); Save.Text = "保存 .diag";
            Open.AutoSize = true; Open.BackColor = Color.White; Open.FlatStyle = FlatStyle.Flat; Open.ForeColor = Color.FromArgb(26,53,73); Open.MinimumSize = new Size(86, 30); Open.Text = "打开 .diag";
            Hint.Size = new Size(420, 26); Hint.Text = "无设备连接时也可以打开并阅读已有诊断文件。"; Hint.TextAlign = ContentAlignment.MiddleLeft;
            DiagnosticText.BackColor = Color.White; DiagnosticText.Dock = DockStyle.Fill; DiagnosticText.Font = new Font("Consolas", 9.5F); DiagnosticText.ReadOnly = true;
            AutoScaleDimensions = new SizeF(7F, 17F); AutoScaleMode = AutoScaleMode.Font; BackColor = Color.White;
            Controls.Add(DiagnosticText); Controls.Add(Tools); Font = new Font("Microsoft YaHei UI", 9F); Name = "DiagnosticView"; Size = new Size(1120, 620);
            Tools.ResumeLayout(false); Tools.PerformLayout(); ResumeLayout(false); PerformLayout();
        }
    }
}
