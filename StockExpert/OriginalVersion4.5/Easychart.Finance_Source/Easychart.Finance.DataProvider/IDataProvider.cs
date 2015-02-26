namespace Easychart.Finance.DataProvider
{
    using Easychart.Finance;
    using System;
    using System.Reflection;

    public interface IDataProvider
    {
        double GetConstData(string DataType);
        string GetStringData(string DataType);
        string GetUnique();

        IDataProvider BaseDataProvider { get; set; }

        int Count { get; }

        Easychart.Finance.DataCycle DataCycle { get; set; }

        IDataManager DataManager { get; }

        double[] this[string Name] { get; }

        int MaxCount { get; set; }
    }
}

