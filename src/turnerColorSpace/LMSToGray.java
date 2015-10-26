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
public class LMSToGray implements ColorspaceToGray{
    
    private BufferedImage LChannel;
    private BufferedImage MChannel;
    private BufferedImage SChannel;
    private WritableRaster Lr;
    private WritableRaster Mr;
    private WritableRaster Sr;
    
    private BufferedImage[] LMSChannels = new BufferedImage[3];
    
    //Testing variables:
//    static double lt;
//    static double mt;
//    static double st;
    
    public LMSToGray(int width, int height){
        LChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        MChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        SChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        
        LMSChannels[0] = LChannel;
        LMSChannels[1] = MChannel;
        LMSChannels[2] = SChannel;
        
        Lr = LChannel.getRaster();
        Mr = MChannel.getRaster();
        Sr = SChannel.getRaster();
    }
    
    public void setPixelColor(LMSColor lms, int x, int y){
        double normalizedL = (lms.getL() - LMSColor.LMin)/(LMSColor.LMax - LMSColor.LMin);
        double normalizedM = (lms.getM() - LMSColor.MMin)/(LMSColor.MMax - LMSColor.MMin);
        double normalizedS = (lms.getS() - LMSColor.SMin)/(LMSColor.SMax - LMSColor.SMin);
        
        Lr.setSample(x, y, 0, (int) (normalizedL*255));
        Mr.setSample(x, y, 0, (int) (normalizedM*255));
        Sr.setSample(x, y, 0, (int) (normalizedS*255));
        
        //testing variables
//        lt = lms.getL();
//        mt = lms.getM();
//        st = lms.getS();
    }

    public void setPixelColor(XYZColor xyz, int x, int y){
        LMSColor lms = new LMSColor(xyz);
        setPixelColor(lms, x, y);
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
        return LMSChannels;
    }

    @Override
    public void writeGrayscaleImages(String prefix) {
        try {
            ImageIO.write(LChannel, "PNG", new File(prefix + "Long.png"));
            ImageIO.write(MChannel, "PNG", new File(prefix + "Medium.png"));
            ImageIO.write(SChannel, "PNG", new File(prefix + "Short.png"));
        } catch (IOException ex) {
            Logger.getLogger(LMSToGray.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println("LMS could not write images");
        }
    }
    
//    public static void main(String[] args) {
//        LMSToGray test = new LMSToGray(1,1);
//        double lmax = 0;
//        double lmin = 0;
//        double mmax = 0;
//        double mmin = 0;
//        double smax = 0;
//        double smin = 0;
//        for(int ri = 0; ri < 256; ri++){
//            for (int gi = 0; gi < 256; gi++){
//                for (int bi = 0; bi < 256; bi++){
//                    test.setPixelColor(new Color(ri, gi, bi), 0, 0);
//                    if (lmax < lt) lmax = lt;
//                    if (lmin > lt) lmin = lt;
//                    if (mmax < mt) mmax = mt;
//                    if (mmin > mt) mmin = mt;
//                    if (smax < st) smax = st;
//                    if (smin > st) smin = st;
//                }
//            }
//        }
//        System.out.println("Lmax = " + lmax);
//        System.out.println("Lmin = " + lmin);
//        System.out.println("Mmax = " + mmax);
//        System.out.println("Mmin = " + mmin);
//        System.out.println("Smax = " + smax);
//        System.out.println("Smin = " + smin);
//    }
}
