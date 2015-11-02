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
import static java.lang.Double.isNaN;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.imageio.ImageIO;

/**
 *
 * @author s14003024
 */
public class LabToGray implements ColorspaceTransform{
    
    private BufferedImage LChannel;
    private BufferedImage aChannel;
    private BufferedImage bChannel;
    private WritableRaster Lr;
    private WritableRaster ar;
    private WritableRaster br;
    //ref variables are White reference, based on https://en.wikipedia.org/wiki/Illuminant_D65 and maximum XYZColor class values for X, Y and Z
    private final static double Xref = .95047;
    private final static double Yref = 1.000;
    private final static double Zref = 1.08883;
    //epsilon and kappa from: http://www.brucelindbloom.com/Eqn_XYZ_to_Luv.html
    private final static double epsilon = 216.0/24389.0;
    private final static double kappa = 24389.0/27.0;
    //max and min values determined by testing and recording max/min values based on every sRGB color
    private final static double maxl = 100.00000386666655;
    private final static double minl = 0.0;
    private final static double maxa = 98.23431188800397;
    private final static double mina = -86.18271642053466;
    private final static double maxb = 94.47797505367026;
    private final static double minb = -107.8601617541481;
    
    //Testing variables:
//    static double lt;
//    static double at;
//    static double bt;
    
    private BufferedImage[] LabChannels = new BufferedImage[3];
    
    public LabToGray(int width, int height){
        LChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        aChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        bChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        
        LabChannels[0] = LChannel;
        LabChannels[1] = aChannel;
        LabChannels[2] = bChannel;
        
        Lr = LChannel.getRaster();
        ar = aChannel.getRaster();
        br = bChannel.getRaster();
    }

    public void setPixelColor(XYZColor xyz, int x, int y){
        double xval = xyz.getX();
        double yval = xyz.getY();
        double zval = xyz.getZ();
        
        double yref = yval/Yref;
        double xref = xval/Xref;
        double zref = zval/Zref;
        
        double fx;
        double fy;
        double fz;
        
        if (xref > epsilon){
            fx = Math.cbrt(xref);
        } else {
            fx = ((kappa * xref)+ 16)/116;
        }
        
        if (yref > epsilon){
            fy = Math.cbrt(yref);
        } else {
            fy = ((kappa * yref)+ 16)/116;
        }
        
        if (zref > epsilon){
            fz = Math.cbrt(zref);
        } else {
            fz = ((kappa * zref)+ 16)/116;
        }
        
        double l = (116 * fy) - 16;
        double a = 500 * (fx - fy);
        double b = 200 * (fy - fz);
        
        double normalizedL = (l-minl)/(maxl-minl);
        double normalizedA = (a-mina)/(maxa-mina);
        double normalizedB = (b-minb)/(maxb-minb);
        
        Lr.setSample(x, y, 0, (int) (normalizedL * 255));
        ar.setSample(x, y, 0, (int) (normalizedA * 255));
        br.setSample(x, y, 0, (int) (normalizedB * 255));
        
        
        //variables for max/min testing
//        lt = l;
//        at = a;
//        bt = b;
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
        return LabChannels;
    }

    @Override
    public void writeGrayscaleImages(String prefix) {
        try {
            ImageIO.write(LChannel, "PNG", new File(prefix + "LabL.png"));
            ImageIO.write(aChannel, "PNG", new File(prefix + "Laba.png"));
            ImageIO.write(bChannel, "PNG", new File(prefix + "Labb.png"));
        } catch (IOException ex) {
            Logger.getLogger(LabToGray.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println("Lab could not write images");
        }
    }
    
    public void writeGrayscaleImages(File location, String prefix) {
        try {
            ImageIO.write(LChannel, "PNG", new File(location, prefix + "LabL.png"));
            ImageIO.write(aChannel, "PNG", new File(location, prefix + "Laba.png"));
            ImageIO.write(bChannel, "PNG", new File(location, prefix + "Labb.png"));
        } catch (IOException ex) {
            Logger.getLogger(LabToGray.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println("Lab could not write images");
        }
    }
    
    public ColorSpace getColorSpace(){
        return ColorSpace.Lab;
    }
    
//    public static void main(String[] args) {
//        LabToGray test = new LabToGray(1,1);
//        double lmax = 0;
//        double lmin = 0;
//        double amax = 0;
//        double amin = 0;
//        double bmax = 0;
//        double bmin = 0;
//        for(int ri = 0; ri < 256; ri++){
//            for (int gi = 0; gi < 256; gi++){
//                for (int bi = 0; bi < 256; bi++){
//                    Color temp = TurnerUtil.invertGamma(new Color(ri, gi, bi));
//                    test.setPixelColor(temp, 0, 0);
//                    if (lmax < lt) lmax = lt;
//                    if (lmin > lt) lmin = lt;
//                    if (amax < at) amax = at;
//                    if (amin > at) amin = at;
//                    if (bmax < bt) bmax = bt;
//                    if (bmin > bt) bmin = bt;
//                }
//            }
//        }
//        System.out.println("Lmax = " + lmax);
//        System.out.println("Lmin = " + lmin);
//        System.out.println("amax = " + amax);
//        System.out.println("amin = " + amin);
//        System.out.println("bmax = " + bmax);
//        System.out.println("bmin = " + bmin);
//    }
//    
}
