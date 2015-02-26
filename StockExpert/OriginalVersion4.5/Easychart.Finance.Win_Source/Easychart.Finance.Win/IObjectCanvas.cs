namespace Easychart.Finance.Win
{
    using Easychart.Finance;
    using System;
    using System.Windows.Forms;

    public interface IObjectCanvas
    {
        FormulaChart BackChart { get; }

        Control DesignerControl { get; }

        bool Designing { get; set; }
    }
}

