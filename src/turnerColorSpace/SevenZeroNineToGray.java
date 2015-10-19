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
public class SevenZeroNineToGray implements ColorspaceToGray{
    
    private BufferedImage YChannel;
    private WritableRaster Yr;
    
    private BufferedImage[] SevenZeroOneChannels = new BufferedImage[1];
    
    public SevenZeroNineToGray(int width, int height){
        YChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        
        SevenZeroOneChannels[0] = YChannel;
        
        Yr = YChannel.getRaster();
    }

    @Override
    public void setPixelColor(int R, int G, int B, int x, int y) {
        double normalizedR = (R * 1.0)/255;
        double normalizedG = (G * 1.0)/255;
        double normalizedB = (B * 1.0)/255;
        double luminance = (.2126 * normalizedR) + (.7152 * normalizedG) + (.0722 * normalizedB);
        Yr.setSample(x, y, 0, (int) (luminance*255));
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
        return SevenZeroOneChannels;
    }

    @Override
    public void writeGrayscaleImages(String prefix) {
        try {
            ImageIO.write(YChannel, "PNG", new File(prefix + "709Y.png"));
        } catch (IOException ex) {
            Logger.getLogger(SevenZeroNineToGray.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println("709 could not write images");
        }
    }
    
}
