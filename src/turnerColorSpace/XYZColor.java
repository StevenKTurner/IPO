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
    
    public XYZColor(Color c){
        double normalizedR = ((c.getRed()*1.0)/255);
        double normalizedG = ((c.getGreen()*1.0)/255);
        double normalizedB = ((c.getBlue()*1.0)/255);
        
        X = (0.41224 * normalizedR) + (0.2576 * normalizedG) + (0.1805 * normalizedB);
        Y = (0.2126 * normalizedR) + (0.7152 * normalizedG) + (0.0722 * normalizedB);
        Z = (0.0193 * normalizedR) + (0.1192 * normalizedG) + (0.9505 * normalizedB);
    }
    
    public XYZColor(int R, int G, int B){
        double normalizedR = ((R*1.0)/255);
        double normalizedG = ((G*1.0)/255);
        double normalizedB = ((B*1.0)/255);
        
        X = (0.41224 * normalizedR) + (0.2576 * normalizedG) + (0.1805 * normalizedB);
        Y = (0.2126 * normalizedR) + (0.7152 * normalizedG) + (0.0722 * normalizedB);
        Z = (0.0193 * normalizedR) + (0.1192 * normalizedG) + (0.9505 * normalizedB);
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
