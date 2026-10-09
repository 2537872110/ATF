namespace CSharpUseC__AF_Dll
{
    partial class ZOutputModeSelector
    {
        private System.ComponentModel.IContainer components = null;
        private System.Windows.Forms.Label modeLabel;
        private System.Windows.Forms.ComboBox modeCombo;
        protected override void Dispose(bool disposing) { if (disposing && components != null) components.Dispose(); base.Dispose(disposing); }
        private void InitializeComponent()
        {
            this.modeLabel = new System.Windows.Forms.Label();
            this.modeCombo = new System.Windows.Forms.ComboBox();
            this.SuspendLayout();
            this.modeLabel.AutoSize = true;
            this.modeLabel.Location = new System.Drawing.Point(4, 7);
            this.modeLabel.Name = "modeLabel";
            this.modeLabel.Text = "输出模式";
            this.modeCombo.Anchor = System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right;
            this.modeCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.modeCombo.Items.AddRange(new object[] { "0 - 脉冲方向（SET_PUL，物理移动）", "1 - 回调输出（输出 Z，不物理移动）", "2 - 模拟量（SET_AI，物理移动）" });
            this.modeCombo.Location = new System.Drawing.Point(72, 3);
            this.modeCombo.Name = "modeCombo";
            this.modeCombo.Size = new System.Drawing.Size(328, 25);
            this.modeCombo.SelectedIndexChanged += new System.EventHandler(this.modeCombo_SelectedIndexChanged);
            this.Controls.Add(this.modeCombo);
            this.Controls.Add(this.modeLabel);
            this.Name = "ZOutputModeSelector";
            this.Size = new System.Drawing.Size(404, 32);
            this.ResumeLayout(false);
            this.PerformLayout();
        }
    }
}
