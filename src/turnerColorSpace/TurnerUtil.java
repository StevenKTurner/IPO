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
    
    public static Color invertGamma(Color c){
        Color InvertC;
        
        InvertC = new Color(deGammatize(c.getRed()), deGammatize(c.getGreen()), deGammatize(c.getBlue()));
        
        return InvertC;
    }
    
    private static int deGammatize(int E){//Gamma Correction based on formula from Poynton.com GammaFAQ
        double normalizedE = ((E*1.0)/255);
        double l;
        
        if (normalizedE <= 0.081){
            l = normalizedE/4.5;
        } else{
            l = (Math.pow((normalizedE + .099)/1.099, (1/0.45)))*255;
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
