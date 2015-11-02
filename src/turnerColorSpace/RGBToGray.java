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
public class RGBToGray implements ColorspaceTransform{
    
    private BufferedImage RChannel;
    private BufferedImage GChannel;
    private BufferedImage BChannel;
    private WritableRaster Rr;
    private WritableRaster Gr;
    private WritableRaster Br;
    
    private BufferedImage[] RGBChannels = new BufferedImage[3];
    
    public RGBToGray(int width, int height){
        RChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        GChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        BChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        
        RGBChannels[0] = RChannel;
        RGBChannels[1] = GChannel;
        RGBChannels[2] = BChannel;
        
        Rr = RChannel.getRaster();
        Gr = GChannel.getRaster();
        Br = BChannel.getRaster();
    }

    @Override
    public void setPixelColor(int R, int G, int B, int x, int y) {
        Rr.setSample(x, y, 0, R);
        Gr.setSample(x, y, 0, G);
        Br.setSample(x, y, 0, B);
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
        return RGBChannels;
    }

    @Override
    public void writeGrayscaleImages(String prefix) {
        try {
            ImageIO.write(RChannel, "PNG", new File(prefix + "RGBChannelR.png"));
            ImageIO.write(GChannel, "PNG", new File(prefix + "RGBChannelG.png"));
            ImageIO.write(BChannel, "PNG", new File(prefix + "RGBChannelB.png"));
        } catch (IOException ex) {
            Logger.getLogger(RGBToGray.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println("RGB could not write images");
        }
    }
    
    public void writeGrayscaleImages(File location, String prefix) {
        try {
            ImageIO.write(RChannel, "PNG", new File(location, prefix + "RGBChannelR.png"));
            ImageIO.write(GChannel, "PNG", new File(location, prefix + "RGBChannelG.png"));
            ImageIO.write(BChannel, "PNG", new File(location, prefix + "RGBChannelB.png"));
        } catch (IOException ex) {
            Logger.getLogger(RGBToGray.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println("RGB could not write images");
        }
    }
    
    public ColorSpace getColorSpace(){
        return ColorSpace.RGB;
    }
}
