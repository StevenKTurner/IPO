/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package turnerColorSpace;

import java.awt.Color;

/**
 *
 * @author s14003024
 */
public class LMSColor {
    private double L;
    private double M;
    private double S;
    
    //max and min figures determined via testing
    public static final double LMax = 0.9737185881980002;
    public static final double LMin = 0;
    public static final double MMax = 1.01550520794;
    public static final double MMin = 0;
    public static final double SMax = 1.08883;
    public static final double SMin = 0;
    
    public LMSColor(XYZColor xyz){
      
        // figures from https://en.wikipedia.org/wiki/LMS_color_space
        L = (.38971 * xyz.getX()) + (.68898 * xyz.getY()) + (-0.07868 * xyz.getZ());
        M = (-.22981 * xyz.getX()) + (1.1834 * xyz.getY()) + (0.04641 * xyz.getZ());
        S = xyz.getZ();
    }
    
    public LMSColor(Color c){
        XYZColor xyz = new XYZColor(c);
        
        // figures from https://en.wikipedia.org/wiki/LMS_color_space
        L = (.38971 * xyz.getX()) + (.68898 * xyz.getY()) + (-0.07868 * xyz.getZ());
        M = (-.22981 * xyz.getX()) + (1.1834 * xyz.getY()) + (0.04641 * xyz.getZ());
        S = xyz.getZ();
    }
    
    public LMSColor(int R, int G, int B){
        XYZColor xyz = new XYZColor(R, G, B);
        
        L = (.38971 * xyz.getX()) + (.68898 * xyz.getY()) + (-0.07868 * xyz.getZ());
        M = (-.22981 * xyz.getX()) + (1.1834 * xyz.getY()) + (0.04641 * xyz.getZ());
        S = xyz.getZ();
    }
    
    public double getL(){
        return L;
    }
    
    public double getM(){
        return M;
    }
    
    public double getS(){
        return S;
    }
    
}
