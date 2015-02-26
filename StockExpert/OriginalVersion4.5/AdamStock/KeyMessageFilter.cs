using System;
using System.Windows.Forms;

namespace StockExpert
{
	public class KeyMessageFilter : IMessageFilter
	{
		// Methods
		public KeyMessageFilter()
		{
		}

		public bool PreFilterMessage(ref Message m)
		{
			if (m.Msg == 0x100)
			{
				KeyEventArgs args1 = new KeyEventArgs(((Keys) m.WParam.ToInt32()) | Control.ModifierKeys);
				ChartForm.HandleKey(args1);
				if (args1.Handled)
				{
					return args1.Handled;
				}
			}
			return false;
		}


		// Fields
		private const int WM_KEYDOWN = 0x100;
	}
 


}
