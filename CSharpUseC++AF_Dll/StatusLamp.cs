using System;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

namespace CSharpUseC__AF_Dll
{
    /// <summary>
    /// 圆形状态指示灯。
    /// 用于传感器状态显示：已对焦 / 在范围 / 硬件错误 / 运动禁止 / 激光启用 /
    /// CW 限位 / CCW 限位 / 相机同步 / LLC / 电机运动中。
    /// </summary>
    public sealed class StatusLamp : Control
    {
        /// <summary>
        /// 状态灯颜色语义。
        /// Gray  = 未启用 / 未触发 / 空闲
        /// Green = 正常 / 已连接 / 运行中 / 已对焦
        /// Red   = 错误 / 故障 / 控制失败
        /// Yellow= 警告 / 限位触发 / 运动禁止
        /// </summary>
        public enum LampState
        {
            Gray,
            Green,
            Red,
            Yellow
        }

        private LampState _state = LampState.Gray;
        private string _caption = string.Empty;

        /// <summary>
        /// 当前灯色状态。改变后自动重绘。
        /// </summary>
        [DefaultValue(LampState.Gray)]
        [Description("灯色状态")]
        [Category("外观")]
        public LampState State
        {
            get { return _state; }
            set
            {
                if (_state == value)
                    return;
                _state = value;
                Invalidate();
            }
        }

        /// <summary>
        /// 灯右侧的状态名称，例如 "已对焦" / "在范围"。
        /// </summary>
        [DefaultValue("")]
        [Description("状态名称")]
        [Category("外观")]
        public string Caption
        {
            get { return _caption; }
            set
            {
                if (string.Equals(_caption, value, StringComparison.Ordinal))
                    return;
                _caption = value ?? string.Empty;
                Invalidate();
            }
        }

        public StatusLamp()
        {
            SetStyle(
                ControlStyles.AllPaintingInWmPaint |
                ControlStyles.OptimizedDoubleBuffer |
                ControlStyles.ResizeRedraw |
                ControlStyles.UserPaint |
                ControlStyles.SupportsTransparentBackColor,
                true);

            BackColor = Color.Transparent;
            Size = new Size(180, 22);
            Font = new Font("Microsoft YaHei UI", 9F, FontStyle.Regular);
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            Graphics g = e.Graphics;
            g.SmoothingMode = SmoothingMode.AntiAlias;
            g.TextRenderingHint = System.Drawing.Text.TextRenderingHint.ClearTypeGridFit;

            // 圆形灯：直径取高度 - 4，居中
            int diameter = Math.Max(10, Height - 6);
            Rectangle lampRect = new Rectangle(2, (Height - diameter) / 2, diameter, diameter);

            Color lampColor = GetColor(_state);

            // 外圈：深色描边
            using (Pen border = new Pen(Color.FromArgb(90, 0, 0, 0), 1.2f))
            {
                g.FillEllipse(CreateBrush(lampColor, lampRect), lampRect);
                g.DrawEllipse(border, lampRect);
            }

            // 内部高光：左上 1/3 偏移的浅色椭圆，模拟 LED 反光
            Rectangle highlight = new Rectangle(
                lampRect.X + 2,
                lampRect.Y + 2,
                lampRect.Width / 3,
                lampRect.Height / 3);
            using (SolidBrush hl = new SolidBrush(Color.FromArgb(110, 255, 255, 255)))
            {
                g.FillEllipse(hl, highlight);
            }

            // 文字：灯右侧
            RectangleF textRect = new RectangleF(
                lampRect.Right + 6,
                0,
                Math.Max(0, Width - lampRect.Right - 8),
                Height);

            using (SolidBrush fore = new SolidBrush(ForeColor))
            {
                StringFormat sf = new StringFormat
                {
                    Alignment = StringAlignment.Near,
                    LineAlignment = StringAlignment.Center,
                    FormatFlags = StringFormatFlags.NoWrap,
                    Trimming = StringTrimming.EllipsisCharacter
                };
                g.DrawString(_caption, Font, fore, textRect, sf);
            }
        }

        private static Color GetColor(LampState s)
        {
            switch (s)
            {
                case LampState.Green: return Color.FromArgb(40, 180, 70);
                case LampState.Red: return Color.FromArgb(220, 60, 60);
                case LampState.Yellow: return Color.FromArgb(245, 190, 40);
                default: return Color.FromArgb(170, 170, 170);
            }
        }

        private static Brush CreateBrush(Color baseColor, Rectangle rect)
        {
            // 径向渐变近似：从左上亮到右下暗
            Color light = ControlPaintLight(baseColor);
            Color dark = ControlPaintDark(baseColor);
            return new LinearGradientBrush(
                rect,
                light,
                dark,
                LinearGradientMode.ForwardDiagonal);
        }

        private static Color ControlPaintLight(Color c)
        {
            return Color.FromArgb(
                Math.Min(255, c.R + 70),
                Math.Min(255, c.G + 70),
                Math.Min(255, c.B + 70));
        }

        private static Color ControlPaintDark(Color c)
        {
            return Color.FromArgb(
                (int)(c.R * 0.65),
                (int)(c.G * 0.65),
                (int)(c.B * 0.65));
        }

    }
}
