namespace Easychart.Finance.Objects
{
    using Easychart.Finance;
    using System;

    public class ObjectSegment
    {
        public ObjectPoint op1;
        public ObjectPoint op2;

        public ObjectSegment(ObjectPoint op1, ObjectPoint op2)
        {
            this.op1 = op1;
            this.op2 = op2;
        }
    }
}

