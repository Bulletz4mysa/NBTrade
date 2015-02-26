// Decompiled by Salamander version 1.1 Beta
// Copyright 2002 Remotesoft Inc. All rights reserved.
// http://www.remotesoft.com/salamander

using System.Reflection;
using Easychart.Finance;
using Easychart.Finance.DataProvider;
using Easychart.Finance.Objects;
using Easychart.Finance.Win;
using System;
using System.Collections;
using System.ComponentModel;
using System.Configuration;
using System.Drawing;
using System.Windows.Forms;

namespace StockExpert
{
    public class ChartForm : Form
    {
        private IContainer components = null;

        private MainMenu mmMain;

        private ObjectManager om;

        private static ChartForm CurrentChartForm;

        private static ArrayList FormList;

        private static KeyMessageFilter KeyFilter;

        private static IDataManager idm;

        public string Code;

        private ContextMenu cmRight;

        private MenuItem miFavorite;

		private MenuItem miUpdate;

        private MenuItem miSp1;

        private MenuItem miFile;

        private MenuItem miPrintSetup;

        private MenuItem miPreview;

        private MenuItem miPrint;

        private MenuItem miSp;

        private MenuItem miSp2;

        private MenuItem miClearObjects;

        public ChartWinControl ChartControl;


        public ChartForm()
        {
			this.components = null;
            InitializeComponent();
            om = new ObjectManager(ChartControl, ToolPanel.NowToolPanel.pg, ToolPanel.NowToolPanel.ObjectToolPanel);
            ChartControl.MergeRightMenu(cmRight);
            if (KeyFilter == null)
            {
                KeyFilter = new KeyMessageFilter();
                Application.AddMessageFilter(KeyFilter);
            }
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing && components != null)
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        private void InitializeComponent()
        {
            AppSettingsReader appSettingsReader = new AppSettingsReader();
            mmMain = new MainMenu();
            miFile = new MenuItem();
            miPrintSetup = new MenuItem();
            miPreview = new MenuItem();
            miPrint = new MenuItem();
            miSp = new MenuItem();
            cmRight = new ContextMenu();
            miSp1 = new MenuItem();
            miFavorite = new MenuItem();
            miSp2 = new MenuItem();
            miClearObjects = new MenuItem();
			this.miUpdate  = new MenuItem ();
            ChartControl = new ChartWinControl();
            base.SuspendLayout();
            mmMain.MenuItems.AddRange(new MenuItem[]{miFile});
            miFile.Index = 0;
            miFile.MenuItems.AddRange(new MenuItem[]{miPrintSetup, miPreview, miPrint, miSp});
            miFile.MergeType = MenuMerge.MergeItems;
            miFile.Text = "&File";
            miPrintSetup.Index = 0;
            miPrintSetup.Text = "&Print Setup";
            miPrintSetup.Click += new EventHandler(this.miPrintSetup_Click);
            miPreview.Index = 1;
            miPreview.Text = "&Print Preview";
            miPreview.Click += new EventHandler(this.miPreview_Click);
            miPrint.Index = 2;
            miPrint.Text = "&Print";
            miPrint.Click += new EventHandler(this.miPrint_Click);
            miSp.Index = 3;
            miSp.Text = "-";
		
            cmRight.MenuItems.AddRange(new MenuItem[]{miSp1, miFavorite, miSp2, miClearObjects});
            miSp1.Index = 0;
            miSp1.MergeOrder = 1000;
            miSp1.Text = "-";
            miFavorite.Index = 1;
            miFavorite.MergeOrder = 1100;
            miFavorite.Text = "Add to &favorite";
            miFavorite.Click += new EventHandler(this.miFavorite_Click);
            miSp2.Index = 2;
            miSp2.MergeOrder = 1200;
            miSp2.Text = "-";
            miClearObjects.Index = 3;
            miClearObjects.MergeOrder = 1300;
            miClearObjects.Text = "&Clear All Objects";
            miClearObjects.Click += new EventHandler(this.miClearObjects_Click);
			miUpdate.Index = 4;
			miUpdate.MergeOrder = 1400;
			miUpdate.Text = "&Update Data";
			miUpdate.Click += new EventHandler(this.miUpdate_Click);
           	
			ChartControl.AreaCount = 5;
            ChartControl.CausesValidation = false;
            ChartControl.ChartDragMode = (ChartDragMode)1;
            ChartControl.ColumnWidth = 7.0;
            ChartControl.DefaultFormulas = (String)appSettingsReader.GetValue("ChartControl.DefaultFormulas", typeof(string));
            ChartControl.Designing = false;
            ChartControl.Dock = DockStyle.Fill;
            ChartControl.FavoriteFormulas = (String)appSettingsReader.GetValue("ChartControl.FavoriteFormulas", typeof(string));
            ChartControl.LatestValueType = (LatestValueType)2;
            ChartControl.Location = new Point(0, 0);
            ChartControl.MaxColumnWidth = 500.0;
            ChartControl.MinColumnWidth = 0.1;
            ChartControl.Name = "ChartControl";
            ChartControl.OverlayFormulas = (String)appSettingsReader.GetValue("ChartControl.OverlayFormulas", typeof(string));
            ChartControl.ShowCrossCursor = true;
            ChartControl.ShowCursorLabel = true;
            ChartControl.ShowIndicatorValues = true;
            ChartControl.ShowOverlayValues = true;
            ChartControl.ShowStatistic = true;
            ChartControl.ShowTopLine = false;
            ChartControl.Size = new Size(744, 549);
            ChartControl.Skin = (String)appSettingsReader.GetValue("ChartControl.Skin", typeof(string));
            ChartControl.StartBar = 0;
            ChartControl.TabIndex = 0;
            ChartControl.DataChanged += new EventHandler(this.ChartControl_DataChanged);
            AutoScaleBaseSize = new Size(6, 14);
            base.ClientSize = new Size(744, 549);
            base.Controls.Add(ChartControl);
            Cursor = Cursors.Default;
            base.KeyPreview = true;
            base.Menu = mmMain;
            base.Name = "ChartForm";
            base.ShowInTaskbar = false;
            Text = "Chart";
            base.WindowState = FormWindowState.Maximized;
            base.KeyDown += new KeyEventHandler(this.ChartForm_KeyDown);
            base.KeyPress += new KeyPressEventHandler(this.ChartForm_KeyPress);
            base.Load += new EventHandler(this.ChartForm_Load);
            base.Closed += new EventHandler(this.ChartForm_Closed);
            base.Activated += new EventHandler(this.ChartForm_Activated);
            base.ResumeLayout(false);
        }

        private void ChartForm_Load(object sender, EventArgs e)
        {
            mmMain.MenuItems.Add(ChartControl.GetEditMenu());
            mmMain.MenuItems.Add(ChartControl.GetViewMenu());
            mmMain.MenuItems.Add(ChartControl.GetChartMenu());
        }

        private void miExit_Click(object sender, EventArgs e)
        {
            base.Close();
        }

        public static void OpenChartForm(string Code, Form Owner, bool NewWindow)
        {
            if (idm == null)
            {
                idm = new FileDataManager(SymbolForm.GetSymbolTable());
            }
            IDataProvider iDataProvider = idm[Code];
            if (iDataProvider != null)
            {
                (iDataProvider as CommonDataProvider).Merge(StockDB.GetRealtimeData(Code));
                if (CurrentChartForm == null || NewWindow)
                {
                    CurrentChartForm = new ChartForm();
                    CurrentChartForm.MdiParent = Owner;
                    CurrentChartForm.Show();
                    FormList.Add(CurrentChartForm);
                }
                CurrentChartForm.om.SaveObject(CurrentChartForm.Code);
                CurrentChartForm.Code = Code;
                CurrentChartForm.ChartControl.ShowChart(iDataProvider);
                CurrentChartForm.Activate();
                CurrentChartForm.om.LoadObject(Code);
            }
        }

        public void RefreshChart()
        {
            CurrentChartForm.ChartControl.NeedRefresh();
        }

        private void ChartForm_Closed(object sender, EventArgs e)
        {
            FormList.Remove(this);
            if (CurrentChartForm == this)
            {
                StatisticForm.FreeStatistic();
                if (FormList.Count > 0)
                {
                    CurrentChartForm = (ChartForm)FormList[0];
                }
                else
                {
                    CurrentChartForm = null;
                }
            }
            om.SaveObject(Code);
        }

        private void ChartForm_Activated(object sender, EventArgs e)
        {
            CurrentChartForm = this;
        }

        private void ChartForm_KeyPress(object sender, KeyPressEventArgs e)
        {
            SymbolForm.PressKeyAndShow(this, e.KeyChar);
        }

        public static void HandleKey(KeyEventArgs e)
        {
			if (((ChartForm.CurrentChartForm != null) && ChartForm.CurrentChartForm.ChartControl.Focused) && (MainForm.MdiMainForm.ActiveMdiChild == ChartForm.CurrentChartForm))
			{
				ChartForm.CurrentChartForm.ChartControl.HandleKeyEvent(e);
			}

        }

        private void ChartForm_KeyDown(object sender, KeyEventArgs e)
        {
			Keys keys1 = e.KeyCode;
			if (keys1 != Keys.Escape)
			{
				switch (keys1)
				{
					case Keys.Prior:
					{
						ListForm.Current.ShowNextChart(-1);
						e.Handled = true;
						return;
					}
					case Keys.Next:
					{
						ListForm.Current.ShowNextChart(1);
						e.Handled = true;
						return;
					}
				}
			}
			else
			{
				ListForm.ShowForm();
				e.Handled = true;
			}

        }

        private void miFavorite_Click(object sender, EventArgs e)
        {
			StockDB.AddToFavorite(this.Code);
        }

        private void ChartControl_DataChanged(object sender, EventArgs e)
        {
			this.ChartControl.RedrawCaption();
        }

        private void miPrintSetup_Click(object sender, EventArgs e)
        {
			this.ChartControl.PrintSetup();

        }

        private void miPreview_Click(object sender, EventArgs e)
        {
			this.ChartControl.PrintPreview();
        }

        private void miPrint_Click(object sender, EventArgs e)
        {
			this.ChartControl.Print();
        }

        private void miClearObjects_Click(object sender, EventArgs e)
        {
			om.Clear();
        }
		private void miUpdate_Click (object sender, EventArgs e)
		{
            string s = InputBox.ShowInputBox("Quote:", "MSFT");
            if (s != "")
            {
                //YahooDataManager ydm = new YahooDataManager();
                //ydm.CacheRoot = Assembly.GetExecutingAssembly().FullName + "Cache";
                //ChartControl..d.DataManager = ydm;
                //ChartControl.EndTime = DateTime.MinValue;
                //ChartControl.Symbol = ChartForm.s;
			}
		}
		static ChartForm()
		{
			ChartForm.FormList = new ArrayList();
		}

    }

}
