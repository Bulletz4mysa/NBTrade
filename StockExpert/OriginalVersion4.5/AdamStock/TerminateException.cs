using System;

namespace StockExpert
{
    public class TerminateException : Exception
    {

        public TerminateException()
        {
        }

        public TerminateException(string Message) : base(Message)
        {
        }
    }

}
