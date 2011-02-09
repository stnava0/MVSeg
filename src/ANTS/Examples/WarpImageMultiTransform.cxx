#include <vector>
#include <string>
#include "itkImageFileReader.h"
#include "itkVector.h"
//#include "itkVectorImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMatrixOffsetTransformBase.h"
#include "itkTransformFactory.h"
#include "itkWarpImageMultiTransformFilter.h"
#include "itkTransformFileReader.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkLinearInterpolateImageFunction.h"


typedef enum{INVALID_FILE=1, AFFINE_FILE, DEFORMATION_FILE, IMAGE_AFFINE_HEADER, IDENTITY_TRANSFORM} TRAN_FILE_TYPE;
typedef struct{
    //    char *filename;
    std::string filename;
    TRAN_FILE_TYPE file_type;
    bool do_affine_inv;

    //    void SetValue(char *filename, TRAN_FILE_TYPE file_type, bool do_affine_inv){
    //        this.filename = filename;
    //        this.file_type = file_type;
    //        this.do_affine_inv = do_affine_inv;
    //    };

} TRAN_OPT;

typedef std::vector<TRAN_OPT> TRAN_OPT_QUEUE;

typedef struct{
    bool use_NN_interpolator;
    bool use_BSpline_interpolator;
    bool use_TightestBoundingBox;
    char * reference_image_filename;
    bool use_RotationHeader;
} MISC_OPT;

void DisplayOptQueue(const TRAN_OPT_QUEUE &opt_queue);
void DisplayOpt(const TRAN_OPT &opt);

TRAN_FILE_TYPE CheckFileType(const char *str){

    std::string filename = str;
    std::string::size_type pos = filename.rfind( "." );
    std::string filepre = std::string( filename, 0, pos );
    if ( pos != std::string::npos ){
        std::string extension = std::string( filename, pos, filename.length()-1);
        if (extension==std::string(".gz")){
            pos = filepre.rfind( "." );
            extension = std::string( filepre, pos, filepre.length()-1 );
        }
        if (extension==".txt") return AFFINE_FILE;
        else return DEFORMATION_FILE;
    }
    else{
        return INVALID_FILE;
    }
    return AFFINE_FILE;
}

bool IsInverseDeformation(const char *str){
    std::string filename = str;
    std::string::size_type pos = filename.rfind( "Inverse" );
    if ( pos == std::string::npos ) return false; 
    else return true;
}


void FilePartsWithgz(const std::string &filename, std::string &path, std::string &name, std::string &ext){
    std::string extension;
    std::string::size_type pos = filename.rfind( "." );
    std::string filepre = std::string( filename, 0, pos );


    if ( pos != std::string::npos ){
        extension = std::string( filename, pos, filename.length()-1);
        if (extension==std::string(".gz")){
            pos = filepre.rfind( "." );
            if (pos != std::string::npos){
                extension = std::string( filepre, pos, filepre.length()-1 ) + ".gz";
                filepre = std::string(filepre, 0, pos);
            }
        }
    } else {
        extension = std::string("");
    }


    ext = extension;

    pos = filepre.rfind('/');

    if ( pos != std::string::npos ){
        path = std::string(filepre, 0, pos+1);
        name = std::string(filepre, pos+1, filepre.length()-1);
    } else {
        path = std::string("");
        name = filepre;

    }

//    std::cout << "filename: " << filename << std::endl
//    << "path: " << path << std::endl
//    << "name: " << name << std::endl
//    << "ext: " << ext << std::endl;

}



bool CheckFileExistence(const char *str){
    std::ifstream myfile(str);
    bool b = myfile.is_open();
    myfile.close();
    return b;
}

void SetAffineInvFlag(TRAN_OPT &opt, bool &set_current_affine_inv){
    opt.do_affine_inv = set_current_affine_inv;
    if (set_current_affine_inv) set_current_affine_inv = false;
}

bool ParseInput(int argc, char **argv, char *&moving_image_filename,
        char *&output_image_filename,
        TRAN_OPT_QUEUE &opt_queue, MISC_OPT &misc_opt){

    opt_queue.clear();
    opt_queue.reserve(argc-2);

    misc_opt.reference_image_filename = NULL;
    misc_opt.use_BSpline_interpolator = false;
    misc_opt.use_TightestBoundingBox = false;
    misc_opt.use_RotationHeader = false;

    misc_opt.use_NN_interpolator=false;
    misc_opt.use_BSpline_interpolator=false;

    moving_image_filename = argv[0];
    output_image_filename = argv[1];

    int ind = 2;
    bool set_current_affine_inv = false;

    while(ind < argc){



        if (strcmp(argv[ind], "--use-NN")==0) {
            misc_opt.use_NN_interpolator = true;
        }
        else if (strcmp(argv[ind], "--use-BSpline")==0) {
            misc_opt.use_BSpline_interpolator = true;
        }
        else if (strcmp(argv[ind], "-R")==0) {
            ind++; if(ind >= argc) return false;
            misc_opt.reference_image_filename = argv[ind];
        }
        else if ((strcmp(argv[ind], "--tightest-bounding-box")==0) &&  (strcmp(argv[ind], "-R")!=0)  ) {
            misc_opt.use_TightestBoundingBox = true;
        }
        else if (strcmp(argv[ind], "--reslice-by-header")==0) {
            misc_opt.use_RotationHeader = true;
            TRAN_OPT opt;
            opt.file_type = IMAGE_AFFINE_HEADER;
            opt.do_affine_inv = false;
            opt_queue.push_back(opt);
        }
        else if (strcmp(argv[ind], "--Id")==0) {
            TRAN_OPT opt;
            opt.filename = "--Id";
            opt.do_affine_inv = false;
            opt.file_type = IDENTITY_TRANSFORM;
            opt_queue.push_back(opt);
        }
        else if (strcmp(argv[ind], "--moving-image-header")==0 || strcmp(argv[ind], "-mh") ==0){
            TRAN_OPT opt;
            opt.file_type = IMAGE_AFFINE_HEADER;
            opt.filename = moving_image_filename;
            //            opt.do_affine_inv = false;
            SetAffineInvFlag(opt, set_current_affine_inv);
            opt_queue.push_back(opt);
        }
        else if (strcmp(argv[ind], "--reference-image-header")==0 || strcmp(argv[ind], "-rh") ==0){
            if (misc_opt.reference_image_filename==NULL){
                std::cout << "reference image filename is not given yet. Specify it with -R before --reference-image-header / -rh." << std::endl;
                return false;
            }

            TRAN_OPT opt;
            opt.file_type = IMAGE_AFFINE_HEADER;
            opt.filename = misc_opt.reference_image_filename;
            //            opt.do_affine_inv = false;
            SetAffineInvFlag(opt, set_current_affine_inv);
            opt_queue.push_back(opt);
        }
        else if (strcmp(argv[ind], "-i")==0) {
            set_current_affine_inv = true;
        }

        else if (strcmp(argv[ind], "--ANTS-prefix")==0){
            ind++;
            std::string prefix = argv[ind];
            std::string path, name, ext;
            FilePartsWithgz(prefix, path, name, ext);
            if (ext=="") ext=".nii.gz";

            std::string deform_file_name, x_deform_name;
            deform_file_name = path+name+std::string("Warp")+ext;
            x_deform_name = path+name+std::string("Warpxvec")+ext;
            if (CheckFileExistence(x_deform_name.c_str())){
                TRAN_OPT opt;
                opt.filename = deform_file_name.c_str();
                opt.file_type = CheckFileType(opt.filename.c_str());
                opt.do_affine_inv = false;
                opt_queue.push_back(opt);
                std::cout << "found deformation file: " << opt.filename << std::endl;
                DisplayOpt(opt);
            }

            std::string affine_file_name;
            affine_file_name = path+name+std::string("Affine.txt");
            if (CheckFileExistence(affine_file_name.c_str())){
                TRAN_OPT opt;
                opt.filename = affine_file_name.c_str();
                opt.file_type = CheckFileType(opt.filename.c_str());
                opt.do_affine_inv = false;
                opt_queue.push_back(opt);
                std::cout << "found affine file: " << opt.filename << std::endl;
                DisplayOpt(opt);
            }

        }
        else if (strcmp(argv[ind], "--ANTS-prefix-invert")==0){
            ind++;
            std::string prefix = argv[ind];
            std::string path, name, ext;
            FilePartsWithgz(prefix, path, name, ext);
            if (ext=="") ext=".nii.gz";

            std::string affine_file_name;
            affine_file_name = path+name+std::string("Affine.txt");
            if (CheckFileExistence(affine_file_name.c_str())){
                TRAN_OPT opt;
                opt.filename = affine_file_name.c_str();
                opt.file_type = CheckFileType(opt.filename.c_str());
                opt.do_affine_inv = true;
                opt_queue.push_back(opt);
                std::cout << "found affine file: " << opt.filename << std::endl;
                DisplayOpt(opt);
            }



            std::string deform_file_name, x_deform_name;
            deform_file_name = path+name+std::string("InverseWarp.nii.gz");
            x_deform_name = path+name+std::string("InverseWarpxvec.nii.gz");
            if (CheckFileExistence(x_deform_name.c_str())){
                TRAN_OPT opt;
                opt.filename = deform_file_name.c_str();
                opt.file_type = CheckFileType(opt.filename.c_str());
                opt.do_affine_inv = false;
                opt_queue.push_back(opt);
                std::cout << "found deformation file: " << opt.filename << std::endl;
                DisplayOpt(opt);
            }
        }
        else{
            TRAN_OPT opt;
            opt.filename = argv[ind];
            opt.file_type = CheckFileType(opt.filename.c_str());
            opt.do_affine_inv = false;
            if (opt.file_type == AFFINE_FILE)
                SetAffineInvFlag(opt, set_current_affine_inv);
            else if (opt.file_type == DEFORMATION_FILE && set_current_affine_inv){
                std::cout << "Ignore inversion of non-affine file type! " << std::endl;
                std::cout << "opt.do_affine_inv:" << opt.do_affine_inv << std::endl;
            }

            opt_queue.push_back(opt);
            DisplayOpt(opt);
        }
        ind++;
    }

    if (misc_opt.use_RotationHeader) {
        //                if (misc_opt.reference_image_filename) {
        //                    opt_queue[0].filename = misc_opt.reference_image_filename;
        //                } else {
        opt_queue[0].filename = "--Id";
        opt_queue[0].file_type = IDENTITY_TRANSFORM;
        opt_queue[0].do_affine_inv = false;
        //                }


        //               TRAN_OPT opt;
        //               opt.file_type = IMAGE_AFFINE_HEADER;
        //               opt.filename = moving_image_filename;
        //               opt.do_affine_inv = true;
        //               opt_queue.push_back(opt);
        //
        //               std::cout << "Use Rotation Header!" << std::endl;

    }

    return true;
}

void DisplayOptQueue(const TRAN_OPT_QUEUE &opt_queue){
    const int kQueueSize = opt_queue.size();
    for(int i=0; i<kQueueSize; i++){
        std::cout << "[" << i << "/" << kQueueSize << "]: ";
        switch(opt_queue[i].file_type){
        case AFFINE_FILE:
            std::cout << "AFFINE";
            break;
        case DEFORMATION_FILE:
            std::cout << "FIELD";
            break;
        case IDENTITY_TRANSFORM:
            std::cout << "IDENTITY";
            break;
        case IMAGE_AFFINE_HEADER:
            std::cout << "HEADER";
            break;
        default:
            std::cout << "Invalid Format!!!";
            break;
        }
        if (opt_queue[i].do_affine_inv) std::cout << "-INV";
        std::cout << ": " << opt_queue[i].filename << std::endl;
    }

}

void DisplayOpt(const TRAN_OPT &opt){
    switch(opt.file_type){
    case AFFINE_FILE:
        std::cout << "AFFINE";
        break;
    case DEFORMATION_FILE:
        std::cout << "FIELD";
        break;
    case IDENTITY_TRANSFORM:
        std::cout << "IDENTITY";
        break;
    case IMAGE_AFFINE_HEADER:
        std::cout << "HEADER";
        break;
    default:
        std::cout << "Invalid Format!!!";
        break;
    }
    if (opt.do_affine_inv) std::cout << "-INV";
    std::cout << ": " << opt.filename << std::endl;
}

template<class AffineTransformPointer>
void GetIdentityTransform(AffineTransformPointer &aff){
    typedef typename AffineTransformPointer::ObjectType AffineTransform;
    aff = AffineTransform::New();
    aff->SetIdentity();
}


template<class ImageTypePointer, class AffineTransformPointer>
void GetAffineTransformFromImage(const ImageTypePointer& img, AffineTransformPointer &aff){
    typedef typename ImageTypePointer::ObjectType ImageType;
    typedef typename ImageType::DirectionType DirectionType;
    typedef typename ImageType::PointType PointType;
    typedef typename ImageType::SpacingType SpacingType;
    typedef typename AffineTransformPointer::ObjectType::TranslationType VectorType;

    DirectionType direction = img->GetDirection();

    SpacingType spacing = img->GetSpacing();
    VectorType translation;
    // translation.Fill(0);
    for(unsigned int i=0; i<ImageType::GetImageDimension(); i++) translation[i]=img->GetOrigin()[i];

    aff->SetMatrix(direction);
    // aff->SetCenter(pt);
    PointType pt; pt.Fill(0);
    aff->SetOffset(translation);
    aff->SetCenter(pt);


    std::cout << "aff from image:" << aff << std::endl;

}


template<class WarperPointerType, class ImagePointerType, class SizeType, class PointType>
void GetLaregstSizeAfterWarp(WarperPointerType &warper, ImagePointerType &img, SizeType &largest_size, PointType &origin_warped){
    typedef typename ImagePointerType::ObjectType ImageType;
    const int ImageDimension = ImageType::GetImageDimension();

    // typedef typename ImageType::PointType PointType;
    typedef typename std::vector<PointType> PointList;

    typedef typename ImageType::IndexType IndexType;

    // PointList pts_orig;
    PointList pts_warped;

    typename ImageType::SizeType imgsz;
    imgsz = img->GetLargestPossibleRegion().GetSize();

    typename ImageType::SpacingType spacing;
    spacing = img->GetSpacing();

    pts_warped.clear();
    if (ImageDimension == 3){

        for(int i=0; i<8; i++){
            IndexType ind;
            switch (i){
            case 0: ind[0]=0; ind[1]=0; ind[2]=0; break;
            case 1: ind[0]=imgsz[0]-1; ind[1]=0; ind[2]=0; break;
            case 2: ind[0]=0; ind[1]=imgsz[1]-1; ind[2]=0; break;
            case 3: ind[0]=imgsz[0]-1; ind[1]=imgsz[1]-1; ind[2]=0; break;
            case 4: ind[0]=0; ind[1]=0; ind[2]=imgsz[2]-1; break;
            case 5: ind[0]=imgsz[0]-1; ind[1]=0; ind[2]=imgsz[2]-1; break;
            case 6: ind[0]=0; ind[1]=imgsz[1]-1; ind[2]=imgsz[2]-1; break;
            case 7: ind[0]=imgsz[0]-1; ind[1]=imgsz[1]-1; ind[2]=imgsz[2]-1; break;
            }
            PointType pt_orig, pt_warped;
            img->TransformIndexToPhysicalPoint(ind, pt_orig);
            if (warper->MultiInverseAffineOnlySinglePoint(pt_orig, pt_warped)==false){
                std::cout << "ERROR: outside of numeric boundary with affine transform." << std::endl;
                exit(-1);
            }
            pts_warped.push_back(pt_warped);
            std::cout << '[' << i << ']' << ind << ',' << pt_orig << "->" << pt_warped << std::endl;
        }
    }
    else if (ImageDimension == 2) {
        for(int i=0; i<4; i++){
            IndexType ind;
            switch (i){
            case 0: ind[0]=0; ind[1]=0;  break;
            case 1: ind[0]=imgsz[0]-1; ind[1]=0;  break;
            case 2: ind[0]=0; ind[1]=imgsz[1]-1;  break;
            case 3: ind[0]=imgsz[0]-1; ind[1]=imgsz[1]-1;  break;
            }
            PointType pt_orig, pt_warped;
            img->TransformIndexToPhysicalPoint(ind, pt_orig);
            if (warper->MultiInverseAffineOnlySinglePoint(pt_orig, pt_warped)==false){
                std::cout << "ERROR: outside of numeric boundary with affine transform." << std::endl;
                exit(-1);
            }
            pts_warped.push_back(pt_warped);
            std::cout << '[' << i << ']' << ind << ',' << pt_orig << "->" << pt_warped << std::endl;
        }

    }
    else {
        std::cout << "could not determine the dimension after warping for non 2D/3D volumes" << std::endl;
        exit(-1);
    }

    PointType pt_min, pt_max;
    pt_min = pts_warped[0];
    pt_max = pts_warped[0];
    for(unsigned int k=0; k<pts_warped.size(); k++){
        for(int i=0; i<ImageDimension; i++){
            pt_min[i] = (pt_min[i]<pts_warped[k][i])?(pt_min[i]):(pts_warped[k][i]);
            pt_max[i] = (pt_max[i]>pts_warped[k][i])?(pt_max[i]):(pts_warped[k][i]);
        }
    }

    for(int i=0; i<ImageDimension; i++){
        largest_size[i] = (int) (ceil((pt_max[i]-pt_min[i]) / spacing[i])+1);
    }

    origin_warped = pt_min;
    std::cout << "origin_warped: " << origin_warped << std::endl;
    std::cout << "pt_min: " << pt_min << " pt_max:" << pt_max << " largest_size:" << largest_size << std::endl;


}


template<int ImageDimension, unsigned int NVectorComponents>
void WarpImageMultiTransform(char *moving_image_filename, char *output_image_filename,
        TRAN_OPT_QUEUE &opt_queue, MISC_OPT &misc_opt){

    typedef itk::Vector<float, NVectorComponents> PixelType;
    typedef itk::Image<PixelType, ImageDimension> ImageType;
    typedef itk::Vector<float, ImageDimension>         VectorType;
    typedef itk::Image<VectorType, ImageDimension>     DeformationFieldType;
    typedef itk::MatrixOffsetTransformBase< double, ImageDimension, ImageDimension > AffineTransformType;
    typedef itk::WarpImageMultiTransformFilter<ImageType,ImageType, DeformationFieldType, AffineTransformType> WarperType;

    itk::TransformFactory<AffineTransformType>::RegisterTransform();

    typedef itk::ImageFileReader<ImageType> ImageFileReaderType;
    typename ImageFileReaderType::Pointer reader_img = ImageFileReaderType::New();
    reader_img->SetFileName(moving_image_filename);
    reader_img->Update();
    typename ImageType::Pointer img_mov = ImageType::New();

    img_mov = reader_img->GetOutput();

    typename ImageType::Pointer img_ref; // = ImageType::New();

    typename ImageFileReaderType::Pointer reader_img_ref = ImageFileReaderType::New();
    if (misc_opt.reference_image_filename){
        reader_img_ref->SetFileName(misc_opt.reference_image_filename);
        reader_img_ref->Update();
        img_ref = reader_img_ref->GetOutput();
    }
    // else
        //    img_ref = NULL;

    typename WarperType::Pointer  warper = WarperType::New();
    warper->SetInput(img_mov);
    PixelType zero; zero.Fill(0);
    warper->SetEdgePaddingValue( zero );



    if (misc_opt.use_NN_interpolator){
        typedef typename itk::NearestNeighborInterpolateImageFunction<ImageType, typename WarperType::CoordRepType> NNInterpolateType;
        typename NNInterpolateType::Pointer interpolator_NN = NNInterpolateType::New();
        std::cout << "User nearest neighbor interpolation (was Haha) " << std::endl;
        warper->SetInterpolator(interpolator_NN);
    }
    else if (misc_opt.use_BSpline_interpolator){
      std::cout << " Not currently supported because of a lack of vector support " << std::endl;
      /*
        typedef typename itk::BSplineInterpolateImageFunction<ImageType, typename WarperType::CoordRepType> BSInterpolateType;
        typename BSInterpolateType::Pointer interpolator_BS = BSInterpolateType::New();
        interpolator_BS->SetSplineOrder(3);
        std::cout << "User B-spline interpolation " << std::endl;
        warper->SetInterpolator(interpolator_BS);
      */
    }
    else {
        typedef typename itk::LinearInterpolateImageFunction<ImageType, typename WarperType::CoordRepType> LinInterpolateType;
        typename LinInterpolateType::Pointer interpolator_LN = LinInterpolateType::New();
        std::cout << "User Linear interpolation " << std::endl;
        warper->SetInterpolator(interpolator_LN);
    }


    typedef itk::TransformFileReader TranReaderType;
    typedef itk::ImageFileReader<DeformationFieldType> FieldReaderType;
    bool takeaffinv=false;
    unsigned int   transcount=0;
    const int kOptQueueSize = opt_queue.size();
    for(int i=0; i<kOptQueueSize; i++){
        const TRAN_OPT &opt = opt_queue[i];

        switch(opt.file_type){
        case AFFINE_FILE:{
            typename TranReaderType::Pointer tran_reader = TranReaderType::New();
            tran_reader->SetFileName(opt.filename);
            tran_reader->Update();
            typename AffineTransformType::Pointer aff = dynamic_cast< AffineTransformType* >
            ((tran_reader->GetTransformList())->front().GetPointer());
            if (opt.do_affine_inv) {
                typename AffineTransformType::Pointer aff_inv = AffineTransformType::New();
                aff->GetInverse(aff_inv);
                aff = aff_inv;
		takeaffinv=true;
            }
            // std::cout <<" aff " << transcount <<  std::endl;
            warper->PushBackAffineTransform(aff);
            if (transcount==0){
                warper->SetOutputSize(img_mov->GetLargestPossibleRegion().GetSize());
                warper->SetOutputSpacing(img_mov->GetSpacing());
                warper->SetOutputOrigin(img_mov->GetOrigin());
                warper->SetOutputDirection(img_mov->GetDirection());
            }
            transcount++;
            break;
        }

        case IDENTITY_TRANSFORM:{
            typename AffineTransformType::Pointer aff;
            GetIdentityTransform(aff);
            // std::cout << " aff id" << transcount << std::endl;
            warper->PushBackAffineTransform(aff);
            transcount++;
            break;
        }

        case IMAGE_AFFINE_HEADER:{

            typename AffineTransformType::Pointer aff = AffineTransformType::New();
            typename ImageType::Pointer img_affine = ImageType::New();
            typename ImageFileReaderType::Pointer reader_image_affine = ImageFileReaderType::New();
            reader_image_affine->SetFileName(opt.filename);
            reader_image_affine->Update();
            img_affine = reader_image_affine->GetOutput();

            GetAffineTransformFromImage(img_affine, aff);

            if (opt.do_affine_inv) {
                typename AffineTransformType::Pointer aff_inv = AffineTransformType::New();
                aff->GetInverse(aff_inv);
                aff = aff_inv;
		takeaffinv=true;
            }

            // std::cout <<" aff from image header " << transcount <<  std::endl;
            warper->PushBackAffineTransform(aff);

            //            if (transcount==0){
            //                warper->SetOutputSize(img_mov->GetLargestPossibleRegion().GetSize());
            //                warper->SetOutputSpacing(img_mov->GetSpacing());
            //                warper->SetOutputOrigin(img_mov->GetOrigin());
            //                warper->SetOutputDirection(img_mov->GetDirection());
            //            }

            transcount++;
            break;
        }

        case DEFORMATION_FILE:{
            typename FieldReaderType::Pointer field_reader = FieldReaderType::New();
            field_reader->SetFileName( opt.filename );
            field_reader->Update();
            typename DeformationFieldType::Pointer field = field_reader->GetOutput();

            warper->PushBackDeformationFieldTransform(field);
            warper->SetOutputSize(field->GetLargestPossibleRegion().GetSize());
            warper->SetOutputOrigin(field->GetOrigin());
            warper->SetOutputSpacing(field->GetSpacing());
            warper->SetOutputDirection(field->GetDirection());

            transcount++;
            break;
        }
        default:
            std::cout << "Unknown file type!" << std::endl;
        }
    }

    //std::cout << " transcount " << transcount << std::endl; warper->PrintTransformList();
    if ( transcount == 2 ) {
      std::cout << "  We check the syntax of your call .... " << std::endl;
      const TRAN_OPT &opt1 = opt_queue[0];
        const TRAN_OPT &opt2 = opt_queue[1];

	if ( opt1.file_type == AFFINE_FILE  && opt2.file_type == DEFORMATION_FILE   ) {
	  bool defisinv=IsInverseDeformation(opt2.filename.c_str()); 
	  if ( ! takeaffinv ) {
	    std::cout << " Your 1st parameter should be an inverse affine map and the 2nd an InverseWarp  --- exiting without applying warp.  Check that , if using an inverse affine map, you pass the -i option before the Affine.txt." << std::endl;
	    return ; 
	  }
	  if ( ! defisinv ) {
	    std::cout << " Your 2nd  parameter should be an InverseWarp when your 1st parameter is an inverse affine map  --- exiting without applying warp.  " << std::endl;
	    return ; 
	  }
	}
	if ( opt2.file_type == AFFINE_FILE  && opt1.file_type == DEFORMATION_FILE   ) {
	  bool defisinv=IsInverseDeformation(opt1.filename.c_str()); 
	  if (  defisinv ) {
	    std::cout << " Your 1st parameter should be a Warp (not Inverse) when your 2nd parameter is an affine map --- exiting without applying warp.  " << std::endl;
	    return ; 
	  }
	  if (  takeaffinv ) {
	    std::cout << " Your 2nd parameter should be a regular affine map (not inverted) if the 1st is a Warp --- exiting without applying warp. " << std::endl;
	    return ; 
	  }
	}
	std::cout <<" syntax probably ok. " << std::endl;
    } else {
      std::cout << " You are doing something more complex -- we wont check syntax in this case " << std::endl;
    }


    if (img_ref.IsNotNull()){
        warper->SetOutputSize(img_ref->GetLargestPossibleRegion().GetSize());
        warper->SetOutputSpacing(img_ref->GetSpacing());
        warper->SetOutputOrigin(img_ref->GetOrigin());
        warper->SetOutputDirection(img_ref->GetDirection());
    }
    else {
        if (misc_opt.use_TightestBoundingBox == true){
            // compute the desired spacking after inputting all the transform files using the

            typename ImageType::SizeType largest_size;
            typename ImageType::PointType origin_warped;
            GetLaregstSizeAfterWarp(warper, img_mov, largest_size, origin_warped);
            warper->SetOutputSize(largest_size);
            warper->SetOutputSpacing(img_mov->GetSpacing());
            warper->SetOutputOrigin(origin_warped);

            typename ImageType::DirectionType d;
            d.SetIdentity();
            warper->SetOutputDirection(d);
        }

    }

    std::cout << "output origin: " << warper->GetOutputOrigin() << std::endl;
    std::cout << "output size: " << warper->GetOutputSize() << std::endl;
    std::cout << "output spacing: " << warper->GetOutputSpacing() << std::endl;
    std::cout << "output direction: " << warper->GetOutputDirection() << std::endl;

    // warper->PrintTransformList();
    warper->DetermineFirstDeformNoInterp();
    warper->Update();

    //    {
    //        typename ImageType::IndexType ind_orig, ind_warped;
    //        ind_orig[0] = 128;
    //        ind_orig[1] = 128;
    //        ind_orig[2] = 16;
    //        typename ImageType::PointType pt_orig, pt_warped;
    //        warper->GetOutput()->TransformIndexToPhysicalPoint(ind_orig, pt_orig);
    //        warper->MultiTransformSinglePoint(pt_orig, pt_warped);
    //        img_mov->TransformPhysicalPointToIndex(pt_warped, ind_warped);
    //        std::cout << "Transform output index " << ind_orig << "("<<pt_orig<<")"
    //        << " from moving image index " << ind_warped << "("<<pt_warped<<")" << std::endl;
    //    }

    //    typename ImageType::PointType pt_in, pt_out;
    //    for(unsigned int i=0; i<ImageDimension; i++){
    //        pt_in[i] = warper->GetOutputSize()[i] * 0.5;
    //    }
    //    warper->MultiTransformSinglePoint(pt_in, pt_out);
    //    std::cout << "pt_in=" << pt_in << " pt_out=" <<pt_out << std::endl;



    typename ImageType::Pointer img_output = ImageType::New();
    img_output=warper->GetOutput();

    typedef itk::ImageFileWriter<ImageType> ImageFileWriterType;
    typename ImageFileWriterType::Pointer writer_img = ImageFileWriterType::New();
    if (img_ref) img_output->SetDirection(img_ref->GetDirection());
    writer_img->SetFileName(output_image_filename);
    writer_img->SetInput(img_output);
    writer_img->Update();
}



int main(int argc, char **argv){

    if (argc<=3)
	{
        std::cout <<  " \n " << std::endl;
	std::cout <<  "Usage: \n " << std::endl;

	//	std::cout << argv[0] <<  " ImageDimension moving_image output_image [-R reference_image | --tightest-bounding-box] (--reslice-by-header) [--use-NN]"
        //<< "[--ANTS-prefix prefix-name | --ANTS-prefix-invert prefix-name] {[deformation_field | [-i] InverseAffineTransform.txt | --Id | [-i] --moving-image-header / -mh  | [-i] --reference-image-header / -rh]} \n" << std::endl;
	std::cout << argv[0] <<  " ImageDimension moving_image output_image  -R reference_image --use-NN   SeriesOfTransformations--(See Below) " << std::endl;
	std::cout <<" SeriesOfTransformations --- " << argv[0] <<  " can apply, via concatenation, an unlimited number of transformations to your data ." << std::endl; 
	std::cout << " Thus, SeriesOfTransformations may be  an Affine transform followed by a warp  another affine and then another warp. " << std::endl;
	std::cout << "  Inverse affine transformations are invoked by calling   -i MyAffine.txt " << std::endl;
	std::cout << " InverseWarps are invoked by passing the InverseWarp.nii.gz  filename (see below for a note about this).  " << std::endl; 
	std::cout << std::endl;
        std::cout << " Example 1: Mapping a warped image into the reference_image domain by applying abcdWarpxvec.nii.gz/abcdWarpyvec.nii.gz/abcdWarpzvec.nii.gz and then abcdAffine.txt\n" << std::endl;

        std::cout << argv[0] <<  " 3 moving_image output_image -R reference_image abcdWarp.nii.gz abcdAffine.txt\n" << std::endl;

        std::cout << " Example 2: To map the fixed/reference_image warped into the moving_image domain by applying the inversion of abcdAffine.txt and then abcdInverseWarpxvec.nii.gz/abcdInverseWarpyvec.nii.gz/abcdInverseWarpzvec.nii.gz .\n" << std::endl;

        std::cout << argv[0] << " 3 reference_image output_image -R moving_image -i  abcdAffine.txt abcdInverseWarp.nii.gz \n \n" << std::endl;
	std::cout <<"  Note that the inverse maps (Ex. 2) are passed to this program in the reverse order of the forward maps (Ex. 1). " << std::endl;
	std::cout <<" This makes sense, geometrically ... see ANTS.pdf for visualization of this syntax." << std::endl;
	std::cout << std::endl;
	std::cout << " Compulsory arguments:\n " << std::endl;

	std::cout << " ImageDimension: 2 or 3 (for 2 or 3 Dimensional registration)\n " << std::endl;

	std::cout << " moving_image: the image to apply the transformation to\n " << std::endl;

	std::cout << " output_image: the resulting image\n \n " << std::endl;

	std::cout << " Optional arguments:\n " << std::endl;

	std::cout << " -R: reference_image space that you wish to warp INTO." << std::endl;
	std::cout << "	   --tightest-bounding-box: Computes the tightest bounding box using all the affine transformations. It will be overrided by -R reference_image if given." << std::endl;
	std::cout << "	   --reslice-by-header: equivalient to -i -mh, or -fh -i -mh if used together with -R. It uses the orientation matrix and origin encoded in the image file header. " << std::endl; 
	std::cout << "	   It can be used together with -R. This is typically not used together with any other transforms.\n " << std::endl;

	std::cout << " --use-NN: Use Nearest Neighbor Interpolation. \n " << std::endl;
	std::cout << " --use-BSpline: Use 3rd order B-Spline Interpolation. \n " << std::endl;

	//	std::cout << " --ANTS-prefix prefix-name: followed by a deformation field filename. \n " << std::endl;

	//	std::cout << " --ANTS-prefix-invert: . \n" << std::endl;

	std::cout << " -i: will use the inversion of the following affine transform. \n " << std::endl;

	//	std::cout << " --Id: use an identity transform. \n " << std::endl;

	// std::cout << " --moving-image-header or -mh: will use the orientation header of the moving image file. This is typically not used with --reslice-by-header.\n " << std::endl;

	//	std::cout << " --reference-image-header or -rh: use the orientation matrix and origin encoded in the image file header. It can be used together with -R.\n " << std::endl;
        std::cout <<  " \n " << std::endl;

	//        std::cout << " For ANTS users:" << std::endl;


        std::cout << " Other Example Usages:" << std::endl;
        std::cout << " Reslice the image: WarpImageMultiTransform 3 Imov.nii.gz Iout.nii.gz --tightest-bounding-box --reslice-by-header" << std::endl;
        std::cout << " Reslice the image to a reference image: WarpImageMultiTransform 3 Imov.nii.gz Iout.nii.gz -R Iref.nii.gz --tightest-bounding-box --reslice-by-header\n" << std::endl;

        std::cout << " Important Notes: " << std::endl;
        std::cout << " Prefixname \"abcd\" without any extension will use \".nii.gz\" by default" << std::endl;
	std::cout << " The abcdWarp and abcdInverseWarp do not exist. They are formed on the basis of abcd(Inverse)Warpxvec/yvec/zvec.nii.gz when calling " << argv[0] <<", yet you have to use them as if they exist." << std::endl;
        exit(0);
    }


    TRAN_OPT_QUEUE opt_queue;
    char *moving_image_filename = NULL;
    char *output_image_filename = NULL;

    MISC_OPT misc_opt;

    bool is_parsing_ok = false;
    int kImageDim = atoi(argv[1]);


    is_parsing_ok = ParseInput(argc-2, argv+2, moving_image_filename, output_image_filename, opt_queue, misc_opt);

    if (is_parsing_ok){
     itk::ImageIOBase::Pointer imageIO =itk::ImageIOFactory::CreateImageIO(moving_image_filename, itk::ImageIOFactory::ReadMode);
     imageIO->SetFileName(moving_image_filename);
     imageIO->ReadImageInformation();
     unsigned int ncomponents=imageIO->GetNumberOfComponents();

        std::cout << "moving_image_filename: " << moving_image_filename << std::endl;
        std::cout << "output_image_filename: " << output_image_filename << std::endl;
        std::cout << "reference_image_filename: ";
        if (misc_opt.reference_image_filename) std::cout << misc_opt.reference_image_filename << std::endl;
        else std::cout << "NULL" << std::endl;
        DisplayOptQueue(opt_queue);

  switch( kImageDim )
   {
   case 2:
     switch( ncomponents )
       {
       case 2:
	 WarpImageMultiTransform<2,2>(moving_image_filename, output_image_filename, opt_queue, misc_opt);
	 break;
       default:
	 WarpImageMultiTransform<2,1>(moving_image_filename, output_image_filename, opt_queue, misc_opt);
	 break;
       }
     break;
   case 3:
     switch( ncomponents )
       {
       case 3:
	 WarpImageMultiTransform<3,3>(moving_image_filename, output_image_filename, opt_queue, misc_opt);
	 break;
       default:
	 WarpImageMultiTransform<3,1>(moving_image_filename, output_image_filename, opt_queue, misc_opt);
	 break;
       }
      break;
   case 4:
     switch( ncomponents )
       {
       case 4:
	 WarpImageMultiTransform<4,4>(moving_image_filename, output_image_filename, opt_queue, misc_opt);
	 break;
       default:
	 WarpImageMultiTransform<4,1>(moving_image_filename, output_image_filename, opt_queue, misc_opt);
	 break;
       }
      break;
   default:
      std::cerr <<" not supported " << kImageDim  << std::endl;
      exit( EXIT_FAILURE );
   }
	  //	  WarpImageMultiTransform<2,2>(moving_image_filename, output_image_filename, opt_queue, misc_opt);

    }
    else{
        std::cout << "Input error!" << std::endl;
    }

    exit(0);


}
