﻿
namespace Stocks.Common
{
    using System;
    using System.Runtime.Serialization;

    [Serializable]
    public class AuthenticationException : Exception
    {
        public AuthenticationException() { }
        public AuthenticationException(string message) : base(message) { }
        public AuthenticationException(string message, Exception inner) : base(message, inner) { }
        protected AuthenticationException( SerializationInfo info, StreamingContext context) : base(info, context) { }
    }
}
