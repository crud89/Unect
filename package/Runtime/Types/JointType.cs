namespace Unect
{

    /// <summary>
    /// Identifies individual joints in tracked bodies.
    /// </summary>
    public enum JointType : int
    {

        SpineBase = 0, 
        SpineMid, 
        Neck, 
        Head,
        ShoulderLeft, 
        ElbowLeft, 
        WristLeft, 
        HandLeft,
        ShoulderRight, 
        ElbowRight, 
        WristRight, 
        HandRight,
        HipLeft, 
        KneeLeft, 
        AnkleLeft, 
        FootLeft,
        HipRight, 
        KneeRight, 
        AnkleRight, 
        FootRight,
        SpineShoulder, 
        HandTipLeft, 
        ThumbLeft, 
        HandTipRight, 
        ThumbRight,
        Count = Body.JointCount

    }

}