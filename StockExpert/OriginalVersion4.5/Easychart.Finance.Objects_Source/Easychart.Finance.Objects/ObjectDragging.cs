namespace Easychart.Finance.Objects
{
    using Easychart.Finance;
    using System;
    using System.Drawing;

    public class ObjectDragging
    {
        public int ControlPointIndex;
        public ObjectPoint[] ControlPoints;
        public BaseObject Object;
        public PointF StartPoint;

        public ObjectDragging(PointF StartPoint, int ControlPointIndex, BaseObject Object)
        {
            this.StartPoint = StartPoint;
            this.ControlPointIndex = ControlPointIndex;
            this.Object = Object;
            this.ControlPoints = (ObjectPoint[]) Object.ControlPoints.Clone();
        }
    }
}

