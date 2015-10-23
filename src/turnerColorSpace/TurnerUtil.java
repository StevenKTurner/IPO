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
public class TurnerUtil {
    
    public static Color invertGamma(Color c){//Inverts gamma on a color, assuming gamma of 2.2
        Color InvertC;
        
        InvertC = new Color(deGammatize(c.getRed(), 2.2), deGammatize(c.getGreen(), 2.2), deGammatize(c.getBlue(), 2.2));
        
        return InvertC;
    }
    
    public static Color invertGamma(Color c, double gamma){//Inverts gamma on a color, allowing you to enter your own gamma
        Color InvertC;
        
        InvertC = new Color(deGammatize(c.getRed(), gamma), deGammatize(c.getGreen(), gamma), deGammatize(c.getBlue(), gamma));
        
        return InvertC;
    }
    
    private static int deGammatize(int E, double gamma){//Gamma Correction based on formula from http://poynton.com/notes/colour_and_gamma/GammaFAQ.html
        double normalizedE = ((E*1.0)/255);
        double l;
        
        if (normalizedE <= 0.081){
            l = normalizedE/(gamma/10);
        } else{
            l = (Math.pow((normalizedE + .099)/1.099, (gamma)))*255;
        }
        
        return (int)l;
    }
    
    public static void main(String[] args) {
        Color test = Color.GRAY;
        Color testRun = invertGamma(test);
        
        System.out.println(test.getRed());
        System.out.println(testRun.getRed());
    }
}
