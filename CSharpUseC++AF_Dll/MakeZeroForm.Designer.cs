namespace CSharpUseC__AF_Dll
{
    partial class MakeZeroForm
    {
        private System.ComponentModel.IContainer components = null;

        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
                components.Dispose();
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        private void InitializeComponent()
        {
            this.splitMain = new System.Windows.Forms.SplitContainer();
            this.grpOptions = new System.Windows.Forms.GroupBox();
            this.rbManual = new System.Windows.Forms.RadioButton();
            this.rbResetRef = new System.Windows.Forms.RadioButton();
            this.rbSetRef = new System.Windows.Forms.RadioButton();
            this.rbSignalOnly = new System.Windows.Forms.RadioButton();
            this.rbAuto = new System.Windows.Forms.RadioButton();
            this.lblHelpCaption = new System.Windows.Forms.Label();
            this.txtHelp = new System.Windows.Forms.TextBox();
            this.pnlButtons = new System.Windows.Forms.Panel();
            this.btnRunAndClose = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.lblMessage = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.splitMain)).BeginInit();
            this.splitMain.Panel1.SuspendLayout();
            this.splitMain.Panel2.SuspendLayout();
            this.splitMain.SuspendLayout();
            this.grpOptions.SuspendLayout();
            this.pnlButtons.SuspendLayout();
            this.SuspendLayout();
            //
            // splitMain
            //
            this.splitMain.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitMain.Location = new System.Drawing.Point(0, 0);
            this.splitMain.Name = "splitMain";
            this.splitMain.Panel1.Controls.Add(this.grpOptions);
            this.splitMain.Panel1.Padding = new System.Windows.Forms.Padding(6);
            this.splitMain.Panel2.Controls.Add(this.lblHelpCaption);
            this.splitMain.Panel2.Controls.Add(this.txtHelp);
            this.splitMain.Panel2.Padding = new System.Windows.Forms.Padding(6);
            this.splitMain.Size = new System.Drawing.Size(660, 380);
            this.splitMain.SplitterDistance = 280;
            this.splitMain.TabIndex = 0;
            //
            // grpOptions
            //
            this.grpOptions.Controls.Add(this.rbAuto);
            this.grpOptions.Controls.Add(this.rbSignalOnly);
            this.grpOptions.Controls.Add(this.rbSetRef);
            this.grpOptions.Controls.Add(this.rbResetRef);
            this.grpOptions.Controls.Add(this.rbManual);
            this.grpOptions.Dock = System.Windows.Forms.DockStyle.Fill;
            this.grpOptions.Location = new System.Drawing.Point(6, 6);
            this.grpOptions.Name = "grpOptions";
            this.grpOptions.Size = new System.Drawing.Size(268, 368);
            this.grpOptions.TabIndex = 0;
            this.grpOptions.TabStop = false;
            this.grpOptions.Text = "操作选项";
            //
            // rbAuto
            //
            this.rbAuto.AutoSize = true;
            this.rbAuto.Location = new System.Drawing.Point(20, 28);
            this.rbAuto.Name = "rbAuto";
            this.rbAuto.Size = new System.Drawing.Size(167, 16);
            this.rbAuto.TabIndex = 0;
            this.rbAuto.Text = "自动零点和信号调整";
            this.rbAuto.UseVisualStyleBackColor = true;
            this.rbAuto.CheckedChanged += new System.EventHandler(this.RadioButton_CheckedChanged);
            //
            // rbSignalOnly
            //
            this.rbSignalOnly.AutoSize = true;
            this.rbSignalOnly.Location = new System.Drawing.Point(20, 56);
            this.rbSignalOnly.Name = "rbSignalOnly";
            this.rbSignalOnly.Size = new System.Drawing.Size(167, 16);
            this.rbSignalOnly.TabIndex = 1;
            this.rbSignalOnly.Text = "仅执行自动信号调整";
            this.rbSignalOnly.UseVisualStyleBackColor = true;
            this.rbSignalOnly.CheckedChanged += new System.EventHandler(this.RadioButton_CheckedChanged);
            //
            // rbSetRef
            //
            this.rbSetRef.AutoSize = true;
            this.rbSetRef.Location = new System.Drawing.Point(20, 84);
            this.rbSetRef.Name = "rbSetRef";
            this.rbSetRef.Size = new System.Drawing.Size(155, 16);
            this.rbSetRef.TabIndex = 2;
            this.rbSetRef.Text = "设置焦点参考位置";
            this.rbSetRef.UseVisualStyleBackColor = true;
            this.rbSetRef.CheckedChanged += new System.EventHandler(this.RadioButton_CheckedChanged);
            //
            // rbResetRef
            //
            this.rbResetRef.AutoSize = true;
            this.rbResetRef.Location = new System.Drawing.Point(20, 112);
            this.rbResetRef.Name = "rbResetRef";
            this.rbResetRef.Size = new System.Drawing.Size(155, 16);
            this.rbResetRef.TabIndex = 3;
            this.rbResetRef.Text = "重置焦点参考位置";
            this.rbResetRef.UseVisualStyleBackColor = true;
            this.rbResetRef.CheckedChanged += new System.EventHandler(this.RadioButton_CheckedChanged);
            //
            // rbManual
            //
            this.rbManual.AutoSize = true;
            this.rbManual.Location = new System.Drawing.Point(20, 140);
            this.rbManual.Name = "rbManual";
            this.rbManual.Size = new System.Drawing.Size(131, 16);
            this.rbManual.TabIndex = 4;
            this.rbManual.Text = "手动设置零点";
            this.rbManual.UseVisualStyleBackColor = true;
            this.rbManual.CheckedChanged += new System.EventHandler(this.RadioButton_CheckedChanged);
            //
            // lblHelpCaption
            //
            this.lblHelpCaption.AutoSize = true;
            this.lblHelpCaption.Font = new System.Drawing.Font("Microsoft YaHei UI", 9F, System.Drawing.FontStyle.Bold);
            this.lblHelpCaption.Location = new System.Drawing.Point(8, 8);
            this.lblHelpCaption.Name = "lblHelpCaption";
            this.lblHelpCaption.Size = new System.Drawing.Size(80, 17);
            this.lblHelpCaption.TabIndex = 0;
            this.lblHelpCaption.Text = "操作说明";
            //
            // txtHelp
            //
            this.txtHelp.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtHelp.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtHelp.Font = new System.Drawing.Font("Microsoft YaHei UI", 9F);
            this.txtHelp.Location = new System.Drawing.Point(6, 28);
            this.txtHelp.Multiline = true;
            this.txtHelp.Name = "txtHelp";
            this.txtHelp.ReadOnly = true;
            this.txtHelp.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtHelp.Size = new System.Drawing.Size(362, 340);
            this.txtHelp.TabIndex = 1;
            this.txtHelp.Text = "操作说明将在加载时填入。";
            //
            // pnlButtons
            //
            this.pnlButtons.Controls.Add(this.btnRunAndClose);
            this.pnlButtons.Controls.Add(this.btnCancel);
            this.pnlButtons.Controls.Add(this.lblMessage);
            this.pnlButtons.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.pnlButtons.Location = new System.Drawing.Point(0, 380);
            this.pnlButtons.Name = "pnlButtons";
            this.pnlButtons.Size = new System.Drawing.Size(660, 60);
            this.pnlButtons.TabIndex = 1;
            //
            // btnRunAndClose
            //
            this.btnRunAndClose.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnRunAndClose.Location = new System.Drawing.Point(430, 18);
            this.btnRunAndClose.Name = "btnRunAndClose";
            this.btnRunAndClose.Size = new System.Drawing.Size(100, 30);
            this.btnRunAndClose.TabIndex = 0;
            this.btnRunAndClose.Text = "运行并关闭";
            this.btnRunAndClose.UseVisualStyleBackColor = true;
            this.btnRunAndClose.Click += new System.EventHandler(this.btnRunAndClose_Click);
            //
            // btnCancel
            //
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(540, 18);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(100, 30);
            this.btnCancel.TabIndex = 1;
            this.btnCancel.Text = "取消";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            //
            // lblMessage
            //
            this.lblMessage.AutoSize = false;
            this.lblMessage.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lblMessage.Font = new System.Drawing.Font("Microsoft YaHei UI", 9F);
            this.lblMessage.ForeColor = System.Drawing.Color.DarkBlue;
            this.lblMessage.Location = new System.Drawing.Point(0, 0);
            this.lblMessage.Name = "lblMessage";
            this.lblMessage.Padding = new System.Windows.Forms.Padding(10, 0, 0, 0);
            this.lblMessage.Size = new System.Drawing.Size(420, 60);
            this.lblMessage.TabIndex = 2;
            this.lblMessage.Text = "就绪。";
            this.lblMessage.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            //
            // MakeZeroForm
            //
            this.AcceptButton = this.btnRunAndClose;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(660, 440);
            this.Controls.Add(this.splitMain);
            this.Controls.Add(this.pnlButtons);
            this.Font = new System.Drawing.Font("Microsoft YaHei UI", 9F);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "MakeZeroForm";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "自动零点和信号设置向导 - Auto Zero and Signal Tune Wizard";
            this.Load += new System.EventHandler(this.MakeZeroForm_Load);
            this.splitMain.Panel1.ResumeLayout(false);
            this.splitMain.Panel2.ResumeLayout(false);
            this.splitMain.Panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitMain)).EndInit();
            this.splitMain.ResumeLayout(false);
            this.grpOptions.ResumeLayout(false);
            this.grpOptions.PerformLayout();
            this.pnlButtons.ResumeLayout(false);
            this.ResumeLayout(false);
        }

        #endregion

        private System.Windows.Forms.SplitContainer splitMain;
        private System.Windows.Forms.GroupBox grpOptions;
        private System.Windows.Forms.RadioButton rbAuto;
        private System.Windows.Forms.RadioButton rbSignalOnly;
        private System.Windows.Forms.RadioButton rbSetRef;
        private System.Windows.Forms.RadioButton rbResetRef;
        private System.Windows.Forms.RadioButton rbManual;
        private System.Windows.Forms.Label lblHelpCaption;
        private System.Windows.Forms.TextBox txtHelp;
        private System.Windows.Forms.Panel pnlButtons;
        private System.Windows.Forms.Button btnRunAndClose;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Label lblMessage;
    }
}
