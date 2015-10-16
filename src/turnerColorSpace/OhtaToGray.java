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
 * @author Steven Turner
 */
public class OhtaToGray implements ColorspaceToGray{
    
    private BufferedImage l1;
    private BufferedImage l2;
    private BufferedImage l3;
    private WritableRaster l1r;
    private WritableRaster l2r;
    private WritableRaster l3r;
    
    private BufferedImage[] ohtaChannels = new BufferedImage[3];
    
    
    public OhtaToGray(int width, int height){
        l1 = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        l2 = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        l3 = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        
        ohtaChannels[0] = l1;
        ohtaChannels[1] = l2;
        ohtaChannels[2] = l3;
        
        l1r = l1.getRaster();
        l2r = l2.getRaster();
        l3r = l3.getRaster();
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
    public void setPixelColor(int R, int G, int B, int x, int y) {
        int l1pix = (R+G+B)/3;
        int l2pix = (R-B)/2;
        int l3pix = ((2*G)-R-B)/4;
        
        l1r.setSample(x, y, 0, l1pix);
        l2r.setSample(x, y, 0, l2pix);
        l3r.setSample(x, y, 0, l3pix);
    }

    @Override
    public BufferedImage[] getGrayscaleImages() {
        return ohtaChannels;
    }

    @Override
    public void writeGrayscaleImages(String prefix) {
        try {
            ImageIO.write(l1, "PNG", new File(prefix + "l1.png"));
            ImageIO.write(l2, "PNG", new File(prefix + "l2.png"));
            ImageIO.write(l3, "PNG", new File(prefix + "l3.png"));
        } catch (IOException ex) {
            Logger.getLogger(OhtaToGray.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println("Ohta could not write images");
        }
    }
    
}
