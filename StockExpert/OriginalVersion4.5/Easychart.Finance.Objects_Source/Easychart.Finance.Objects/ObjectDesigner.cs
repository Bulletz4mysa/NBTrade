namespace Easychart.Finance.Objects
{
    using Easychart.Finance;
    using Easychart.Finance.DataProvider;
    using Easychart.Finance.Win;
    using System;
    using System.ComponentModel;
    using System.Drawing;
    using System.Windows.Forms;

    public class ObjectDesigner : UserControl, IObjectCanvas
    {
        private FormulaChart chart = new FormulaChart();
        private Container components = null;
        private IDataProvider DataProvider = new RandomDataProvider(0x3e8);
        private Brush DefaultBrush = Brushes.Black;
        private Font DesignerFont = new Font("verdana", 10f);
        private bool designing;

        public ObjectDesigner()
        {
            this.InitializeComponent();
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing && (this.components != null))
            {
                this.components.Dispose();
            }
            base.Dispose(disposing);
        }

        private void InitializeComponent()
        {
            this.BackColor = SystemColors.ControlLightLight;
            base.Name = "ObjectDesigner";
            base.Size = new Size(560, 0x1a8);
        }

        protected override void OnPaintBackground(PaintEventArgs pevent)
        {
        }

        public FormulaChart BackChart
        {
            get
            {
                return this.chart;
            }
        }

        public Control DesignerControl
        {
            get
            {
                return this;
            }
        }

        public bool Designing
        {
            get
            {
                return this.designing;
            }
            set
            {
                this.designing = value;
            }
        }
    }
}

