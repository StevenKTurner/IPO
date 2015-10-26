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
public class XYZColor {
    double X;
    double Y;
    double Z;
    public static final double xMax = 0.95047;
    public static final double xMin = 0;
    public static final double yMax = 1.0000001;
    public static final double yMin = 0;
    public static final double zMax = 1.08883;
    public static final double zMin = 0;
    
    public XYZColor(Color c){
        double normalizedR = ((c.getRed()*1.0)/255.0);
        double normalizedG = ((c.getGreen()*1.0)/255.0);
        double normalizedB = ((c.getBlue()*1.0)/255.0);
        
        // figures from http://www.brucelindbloom.com/Eqn_RGB_XYZ_Matrix.html, sRGB D65
        X = (0.4124564 * normalizedR) + (0.3575761 * normalizedG) + (0.1804375 * normalizedB);
        Y = (0.2126729 * normalizedR) + (0.7151522 * normalizedG) + (0.0721750 * normalizedB);
        Z = (0.0193339 * normalizedR) + (0.1191920 * normalizedG) + (0.9503041 * normalizedB);
    }
    
    public XYZColor(int R, int G, int B){
        double normalizedR = ((R*1.0)/255.0);
        double normalizedG = ((G*1.0)/255.0);
        double normalizedB = ((B*1.0)/255.0);
        
        // figures from http://www.brucelindbloom.com/Eqn_RGB_XYZ_Matrix.html, sRGB D65
        X = (0.4124564 * normalizedR) + (0.3575761 * normalizedG) + (0.1804375 * normalizedB);
        Y = (0.2126729 * normalizedR) + (0.7151522 * normalizedG) + (0.0721750 * normalizedB);
        Z = (0.0193339 * normalizedR) + (0.1191920 * normalizedG) + (0.9503041 * normalizedB);
    }
    
    public double getX(){
        return X;
    }
    
    public double getY(){
        return Y;
    }
    
    public double getZ(){
        return Z;
    }
    
}
