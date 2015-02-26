namespace Easychart.Finance.Objects
{
    using Easychart.Finance;
    using Easychart.Finance.Win;
    using System;
    using System.Windows.Forms;

    public class WebCanvas : IObjectCanvas
    {
        private FormulaChart backChart;

        public WebCanvas(FormulaChart fc)
        {
            this.backChart = fc;
        }

        public FormulaChart BackChart
        {
            get
            {
                return this.backChart;
            }
        }

        public Control DesignerControl
        {
            get
            {
                return null;
            }
        }

        public bool Designing
        {
            get
            {
                return false;
            }
            set
            {
            }
        }
    }
}

