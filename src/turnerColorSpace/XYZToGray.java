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
public class XYZToGray implements ColorspaceToGray{
    
    private BufferedImage XChannel;
    private BufferedImage YChannel;
    private BufferedImage ZChannel;
    private WritableRaster Xr;
    private WritableRaster Yr;
    private WritableRaster Zr;
    
    private BufferedImage[] XYZChannels = new BufferedImage[3];
    
    //Testing variables:
//    static double xt;
//    static double yt;
//    static double zt;
    
    public XYZToGray(int width, int height){
        XChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        YChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        ZChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        
        XYZChannels[0] = XChannel;
        XYZChannels[1] = YChannel;
        XYZChannels[2] = ZChannel;
        
        Xr = XChannel.getRaster();
        Yr = YChannel.getRaster();
        Zr = ZChannel.getRaster();
    }

    public void setPixelColor(XYZColor xyz, int x, int y){
        double normalizedX = (xyz.getX()-XYZColor.xMin)/(XYZColor.xMax-XYZColor.xMin);
        double normalizedY = (xyz.getY()-XYZColor.yMin)/(XYZColor.yMax-XYZColor.yMin);
        double normalizedZ = (xyz.getZ()-XYZColor.zMin)/(XYZColor.zMax-XYZColor.zMin);
        
        Xr.setSample(x, y, 0, (int) (normalizedX*255));
        Yr.setSample(x, y, 0, (int) (normalizedY*255));
        Zr.setSample(x, y, 0, (int) (normalizedZ*255));
        
        //testing variables
//        xt = xyz.getX();
//        yt = xyz.getY();
//        zt = xyz.getZ();
    }
    
    @Override
    public void setPixelColor(int R, int G, int B, int x, int y) {
        setPixelColor(new XYZColor(R, G, B), x, y);
    }

    @Override
    public void setPixelColor(Color c, int x, int y) {
        setPixelColor(new XYZColor(c), x, y);
    }

    @Override
    public void setPixelColor(int rgb, int x, int y) {
        Color c = new Color(rgb);
        setPixelColor(c, x, y);
    }

    @Override
    public BufferedImage[] getGrayscaleImages() {
        return XYZChannels;
    }

    @Override
    public void writeGrayscaleImages(String prefix) {
        try {
            ImageIO.write(XChannel, "PNG", new File(prefix + "ChannelX.png"));
            ImageIO.write(YChannel, "PNG", new File(prefix + "ChannelY.png"));
            ImageIO.write(ZChannel, "PNG", new File(prefix + "ChannelZ.png"));
        } catch (IOException ex) {
            Logger.getLogger(XYZToGray.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println("XYZ could not write images");
        }
    }
    
//    public static void main(String[] args) {
//        XYZToGray test = new XYZToGray(1,1);
//        double xmax = 0;
//        double xmin = 0;
//        double ymax = 0;
//        double ymin = 0;
//        double zmax = 0;
//        double zmin = 0;
//        for(int ri = 0; ri < 256; ri++){
//            for (int gi = 0; gi < 256; gi++){
//                for (int bi = 0; bi < 256; bi++){
//                    Color temp = TurnerUtil.invertGamma(new Color(ri, gi, bi));
//                    test.setPixelColor(temp, 0, 0);
//                    if (xmax < xt) xmax = xt;
//                    if (xmin > xt) xmin = xt;
//                    if (ymax < yt) ymax = yt;
//                    if (ymin > yt) ymin = yt;
//                    if (zmax < zt) zmax = zt;
//                    if (zmin > zt) zmin = zt;
//                }
//            }
//        }
//        System.out.println("Xmax = " + xmax);
//        System.out.println("Xmin = " + xmin);
//        System.out.println("Ymax = " + ymax);
//        System.out.println("Ymin = " + ymin);
//        System.out.println("Zmax = " + zmax);
//        System.out.println("Zmin = " + zmin);
//    }
    
}
