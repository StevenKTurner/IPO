/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package turnerColorSpace;

import java.awt.Color;
import java.awt.image.BufferedImage;
import java.awt.image.WritableRaster;
import java.io.File;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.imageio.ImageIO;

/**
 *
 * @author s14003024
 */
public class HSIToGray implements ColorspaceToGray{
    
    private BufferedImage SChannel;
    private BufferedImage IChannel;
    private WritableRaster Sr;
    private WritableRaster Ir;
    
    private BufferedImage[] SIChannels = new BufferedImage[2];
    
    public HSIToGray(int width, int height){
        SChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        IChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        
        SIChannels[1] = SChannel;
        SIChannels[2] = IChannel;
        
        Sr = SChannel.getRaster();
        Ir = IChannel.getRaster();
    }

    @Override
    public void setPixelColor(int R, int G, int B, int x, int y) {
        double S;
        double I;
        double Rcorrected = (R*1.0)/255;
        double Gcorrected = (G*1.0)/255;
        double Bcorrected = (B*1.0)/255;
        double max = Math.max(Bcorrected, Math.max(Rcorrected, Gcorrected));
        double min = Math.min(Bcorrected, Math.min(Rcorrected, Gcorrected));
        
        I = (Rcorrected + Gcorrected + Bcorrected)/3;
        
        if ((max-min) == 0){
            S = 0;
        } else{
            S = 1-(min/I);
        }
        
        Sr.setSample(x, y, 0, S*255);
        Ir.setSample(x, y, 0, I*255);
    }

    @Override
    public void setPixelColor(Color c, int x, int y) {
        int R = c.getRed();
        int G = c.getGreen();
        int B = c.getBlue();
        
        setPixelColor(R, G, B, x, y);
    }

    @Override
    public void setPixelColor(int rgb, int x, int y) {
        Color c = new Color(rgb);
        setPixelColor(c, x, y);
    }

    @Override
    public BufferedImage[] getGrayscaleImages() {
        return SIChannels;
    }

    @Override
    public void writeGrayscaleImages(String prefix) {
        try {
            ImageIO.write(SChannel, "PNG", new File(prefix + "HSISaturation.png"));
            ImageIO.write(IChannel, "PNG", new File(prefix + "HSIIntensity.png"));
        } catch (IOException ex) {
            Logger.getLogger(HSIToGray.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println("HSI could not write images");
        }
    }
    
}
