/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package turnerColorSpace;

import java.awt.image.BufferedImage;
import java.awt.image.Raster;

/**
 *
 * @author s14003024
 */
public class OtsuThreshold implements Threshold{
    int thresholdNumber;
    int[] histogram;
    
    public OtsuThreshold(){
        thresholdNumber = 0;
        histogram = new int[256];
    }

    public int getThresholdInt(BufferedImage b) {
        populateHistogram(b);
        
        //Otsu algorithm from http://www.labbookpages.co.uk/software/imgProc/otsuThreshold.html
        int total = b.getHeight() * b.getWidth();
        double sum = 0;
        
        for (int i = 1; i < 256; i++){
            sum += i * histogram[i];
        }
        
        double sumB = 0;
        int wB = 0;
        int wF = 0;
        
        double varMax = 0;
        thresholdNumber = 0;
        
        for (int i = 0; i < 256; i++){
            wB += histogram[i];
            if (wB == 0) continue;
            
            wF = total - wB;
            if (wF == 0) break;
            
            sumB += (double) (i * histogram[i]);
            
            double mB = sumB/wB;
            double mF = (sum - sumB)/wF;
            
            double varBetween = (double)wB * (double)wF * (mB - mF) * (mB - mF);
            
            if (varBetween > varMax){
                varMax = varBetween;
                thresholdNumber = i;
            }
        }
        
        return thresholdNumber;
    }
    
    private void clearHistogram(){
        for (int i = 0; i < histogram.length; i++){
            histogram[i] = 0;
        }
    }
    
    private void populateHistogram(BufferedImage b){
        clearHistogram();
        
        Raster br = b.getRaster();
        for (int h = 0; h < b.getHeight(); h++){
            for (int w = 0; w < b.getWidth(); w++){
                //get the gray value from the image raster
                int value = br.getSample(w, h, 0);
                histogram[value]++;
            }
        }
    }
    
    
}
