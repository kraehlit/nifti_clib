#define _NIFTI1_IO_C_

#include "nifti1_io.h"   /*** typedefs, prototypes, macros, etc. ***/

/*****===================================================================*****/
/*****     Sample functions to deal with NIFTI-1 and ANALYZE files       *****/
/*****...................................................................*****/
/*****            This code is released to the public domain.            *****/
/*****...................................................................*****/
/*****  Author: Robert W Cox, SSCC/DIRP/NIMH/NIH/DHHS/USA/EARTH          *****/
/*****  Date:   August 2003                                              *****/
/*****...................................................................*****/
/*****  Neither the National Institutes of Health (NIH), nor any of its  *****/
/*****  employees imply any warranty of usefulness of this software for  *****/
/*****  any purpose, and do not assume any liability for damages,        *****/
/*****  incidental or otherwise, caused by any use of this document.     *****/
/*****===================================================================*****/


/* global version and history strings, for printing */
static char gni_version[] = "nifti library version 0.8 (December 21, 2004)";
static char gni_history[] = 
  "----------------------------------------------------------------------\n"
  "history (of nifti library changes):\n"
  "\n"
  "0.0  August, 2003 [rwcox]\n"
  "     (Robert W Cox of the National Institutes of Health, SSCC/DIRP/NIMH)\n"
  "   - initial version\n"
  "\n"
  "0.1  July/August, 2004 [Mark Jenkinson]\n"
  "     (FMRIB Centre, University of Oxford, UK)\n"
  "   - Mainly adding low-level IO and changing things to allow gzipped\n"
  "     files to be read and written\n"
  "   - Full backwards compatability should have been maintained\n"
  "\n"
  "0.2  16 Nov 2004 [rickr]\n"
  "     (Rick Reynolds of the National Institutes of Health, SSCC/DIRP/NIMH)\n"
  "   - included Mark's changes in the AFNI distribution (including znzlib/)\n"
  "     (HAVE_ZLIB is commented out for the standard distribution)\n"
  "   - modified nifti_validfilename() and nifti_makebasename()\n"
  "   - added nifti_find_file_extension()\n"
  "\n"
  "0.3  3 Dec 2004 [rickr]\n"
  "   - note: header extensions are not yet checked for\n"
  "   - added formatted history as global string, for printing\n"
  "   - added nifti_disp_lib_hist(), to display the nifti library history\n"
  "   - added nifti_disp_lib_version(), to display the nifti library history\n"
  "   - re-wrote nifti_findhdrname()\n"
  "       o used nifti_find_file_extension()\n"
  "       o changed order of file tests (default is .nii, depends on input)\n"
  "       o free hdrname on failure\n"
  "   - made similar changes to nifti_findimgname()\n"
  "   - check for NULL return from nifti_findhdrname() calls\n"
  "   - removed most of ERREX() macros\n"
  "   - modified nifti_image_read()\n"
  "       o added debug info and error checking (on gni_debug > 0, only)\n"
  "       o fail if workingname is NULL\n"
  "       o check for failure to open header file\n"
  "       o free workingname on failure\n"
  "       o check for failure of nifti_image_load()\n"
  "       o check for failure of nifti_convert_nhdr2nim()\n"
  "   - changed nifti_image_load() to int, and check nifti_read_buffer return\n"
  "   - changed nifti_read_buffer() to fail on short read, and to count float\n"
  "     fixes (to print on debug)\n"
  "   - changed nifti_image_infodump to print to stderr\n"
  "   - updated function header comments, or moved comments above header\n"
  "   - removed const keyword\n"
  "   - added LNI_FERR() macro for error reporting on input files\n"
  "\n"
  "0.4  10 Dec 2004 [rickr]  - added header extensions\n"
  "   - in nifti1_io.h:\n"
  "       o added num_ext and ext_list to the definition of nifti_image\n"
  "       o made many functions static (more to follow)\n"
  "       o added LNI_MAX_NIA_EXT_LEN, for max nifti_type 3 extension length\n"
  "   - added __DATE__ to version output in nifti_disp_lib_version()\n"
  "   - added nifti_disp_matrix_orient() to print orientation information\n"
  "   - added '.nia' as a valid file extension in nifti_find_file_extension()\n"
  "   - added much more debug output\n"
  "   - in nifti_image_read(), in the case of an ASCII header, check for\n"
  "     extensions after the end of the header\n"
  "   - added nifti_read_extensions() function\n"
  "   - added nifti_read_next_extension() function\n"
  "   - added nifti_add_exten_to_list() function\n"
  "   - added nifti_valid_extension() function\n"
  "   - added nifti_write_extensions() function\n"
  "   - added nifti_extension_size() function\n"
  "   - in nifti_set_iname_offest():\n"
  "       o adjust offset by the extension size and the extender size\n"
  "       o fixed the 'ceiling modulo 16' computation\n"
  "   - in nifti_image_write_hdr_img2(): \n"
  "       o added extension writing\n"
  "       o check for NULL return from nifti_findimgname()\n"
  "   - include number of extensions in nifti_image_to_ascii() output\n"
  "   - in nifti_image_from_ascii():\n"
  "       o return bytes_read as a parameter, computed from the final spos\n"
  "       o extract num_ext from ASCII header\n"
  "\n"
  "0.5  14 Dec 2004 [rickr]  - added sub-brick reading functions\n"
  "   - added nifti_brick_list type to nifti1_io.h, along with new prototypes\n"
  "   - added main nifti_image_read_bricks() function, with description\n"
  "   - added nifti_image_load_bricks() - library function (requires nim)\n"
  "   - added valid_nifti_brick_list() - library function\n"
  "   - added free_NBL() - library function\n"
  "   - added update_nifti_image_for_brick_list() for dimension update\n"
  "   - added nifti_load_NBL_bricks(), nifti_alloc_NBL_mem(),\n"
  "           nifti_copynsort() and force_positive() (static functions)\n"
  "   - in nifti_image_read(), check for failed load only if read_data is set\n"
  "   - broke most of nifti_image_load() into nifti_image_load_prep()\n"
  "\n"
  "0.6  15 Dec 2004 [rickr]  - added sub-brick writing functionality\n"
  "   - in nifti1_io.h, removed znzlib directory from include - all nifti\n"
  "       library files are now under the nifti directory\n"
  "   - nifti_read_extensions(): print no offset warning for nifti_type 3\n"
  "   - nifti_write_all_data():\n"
  "       o pass nifti_brick_list * NBL, for optional writing\n"
  "       o if NBL, write each sub-brick, sequentially\n"
  "   - nifti_set_iname_offset(): case 1 must have sizeof() cast to int\n"
  "   - pass NBL to nifti_image_write_hdr_img2(), and allow NBL or data\n"
  "   - added nifti_image_write_bricks() wrapper for ...write_hdr_img2()\n"
  "   - included compression abilities\n"
  "\n"
  "0.7  16 Dec 2004 [rickr] - minor changes to extension reading\n"
  "\n"
  "0.8  21 Dec 2004 [rickr] - restrict extension reading, and minor changes\n"
  "   - in nifti_image_read(), compute bytes for extensions (see remaining)\n"
  "   - in nifti_read_extensions(), pass 'remain' as space for extensions,\n"
  "        pass it to nifti_read_next_ext(), and update for each one read \n"
  "   - in nifti_valid_extension(), require (size <= remain)\n"
  "   - in update_nifti_image_brick_list(), update nvox\n"
  "   - in nifti_image_load_bricks(), make explicit check for nbricks <= 0\n"
  "   - in int_force_positive(), check for (!list)\n"
  "   - in swap_nifti_header(), swap sizeof_hdr, and reorder to struct order\n"
  "   - change get_filesize functions to signed ( < 0 is no file or error )\n"
  "   - in nifti_valid_filename(), lose redundant (len < 0) check\n"
  "   - make print_hex_vals() static\n"
  "   - in disp_nifti_1_header, restrict string field widths\n"
  "----------------------------------------------------------------------\n";

/* global debug level */
static int gni_debug = 0;

/* for calling from some main program */
void nifti_disp_lib_hist( void )
{
   fputs(gni_history, stdout);
}

void nifti_disp_lib_version( void )
{
   printf("%s, compiled %s\n", gni_version, __DATE__);
}


/*----------------------------------------------------------------------
 * nifti_image_read_bricks          - read nifti data as array of bricks
 *                                    13 Dec 2004 [rickr]
 * inputs:
 *          hname    - filename of dataset to read
 *                   - must be valid
 *          nbricks  - number of sub-bricks to read
 *                   - if blist is valid, nbricks must be > 0
 *          blist    - list of sub-bricks to read (can be NULL)
 *                   - if NULL, read complete dataset
 *          NBL      - pointer to empty nifti_brick_list struct
 *                   - must be a valid pointer
 *
 * return:
 *          nim      - same as nifti_image_read, but data is always NULL
 *          NBL      - filled with data
 *
 * By default, this function will read the nifti dataset and break the data
 * into a list of nt*nu*nv*nw sub-bricks, each having size nx*ny*nz elements.
 * That is to say, instead of reading the entire dataset as a single array,
 * break it up into sub-bricks, each of size nx*ny*nz elements.
 *
 * If 'blist' is valid, it is taken to be a list of sub-bricks, of length
 * 'nbricks'.  The data will still be separated into sub-bricks of size
 * nx*ny*nz elements, but now 'nbricks' sub-bricks will be returned, of the
 * caller's choosing via 'blist'.
 *
 * E.g. consider a dataset with 12 sub-bricks (numbered 0..11), and the
 * following code:
 *
 * { nifti_brick_list   NB_orig, NB_select;
 *   nifti_image      * nim_orig, * nim_select;
 *   int                blist[5] = { 7, 0, 5, 5, 9 };
 *
 *   nim_orig   = nifti_image_read_bricks("myfile.nii", 0, NULL,  &NB_orig);
 *   nim_select = nifti_image_read_bricks("myfile.nii", 5, blist, &NB_select);
 * }
 *
 * Here, nim_orig gets the entire dataset, where NB_orig.nbricks = 11.  But
 * nim_select has NB_select.nbricks = 5.
 *
 * Note that the first case is not quite the same as just calling the
 * nifti_image_read function, as here the data is separated into sub-bricks.
 *
 * Note that valid values for blist are in [0..nt*nu*nv*nw-1],
 * or written [ 0 .. (dim[4]*dim[5]*dim[6]*dim[7] - 1) ].
 *----------------------------------------------------------------------*/
nifti_image *nifti_image_read_bricks( char *hname, int nbricks, int * blist,
                                      nifti_brick_list * NBL )
{
   nifti_image * nim;

   if( !hname || !NBL ){
      fprintf(stderr,"** nifti_image_read_bricks: bad params (%p,%p)\n",
              hname, NBL);
      return NULL;
   }

   if( blist && nbricks <= 0 ){
      fprintf(stderr,"** nifti_image_read_bricks: bad nbricks, %d\n", nbricks);
      return NULL;
   }

   nim = nifti_image_read(hname, 0);  /* read header, but not data */

   if( !nim ) return NULL;   /* errors were already printed */

   /* if we fail, free image and return */
   if( nifti_image_load_bricks(nim, nbricks, blist, NBL) <= 0 ){
      nifti_image_free(nim);
      return NULL;
   }

   if( blist ) update_nifti_image_for_brick_list(nim, nbricks);

   return nim;
}


/*----------------------------------------------------------------------
 * update_nifti_image_for_brick_list  - update nifti_image
 *
 * When loading a specific brick list, the distinction between
 * nt, nu, nv and nw is lost.  So put everything in t, and set
 * dim[0] = 4.
 *----------------------------------------------------------------------*/
static void update_nifti_image_for_brick_list( nifti_image * nim , int nbricks )
{
   int ndim;

   if( gni_debug > 2 ){
      fprintf(stderr,"+d updating image dimensions for %d bricks in list\n",
              nbricks);
      fprintf(stderr,"   ndim = %d\n",nim->ndim);
      fprintf(stderr,"   nx,ny,nz,nt,nu,nv,nw: (%d,%d,%d,%d,%d,%d,%d)",
              nim->nx, nim->ny, nim->nz, nim->nt, nim->nu, nim->nv, nim->nw);
   }

   nim->nt = nbricks;
   nim->nu = nim->nv = nim->nw = 1;

   nim->nvox =  nim->nx * nim->ny * nim->nz
              * nim->nt * nim->nu * nim->nv * nim->nw;

   nim->dim[4] = nbricks;
   nim->dim[5] = nim->dim[6] = nim->dim[7] = 1;

   /* update the dimensions to 4 or lower */
   for( ndim = 4; (ndim > 1) && (nim->dim[ndim] <= 1); ndim-- )
       ;

   if( gni_debug > 2 ){
      fprintf(stderr,"+d ndim = %d -> %d\n",nim->ndim, ndim);
      fprintf(stderr," --> (%d,%d,%d,%d,%d,%d,%d)\n",
              nim->nx, nim->ny, nim->nz, nim->nt, nim->nu, nim->nv, nim->nw);
   }

   nim->dim[0] = nim->ndim = ndim;
}


/*----------------------------------------------------------------------
 * Load the image data from disk into an already-prepared image struct.
 *                                                  13 Dec 2004 [rickr]
 * 
 * If blist is NULL, read the dataset normally.
 * 
 * return the number of loaded bricks (NBL->nbricks)
 *
 *  > 0 : success
 *    0 : failure
 *  < 0 : error
 *----------------------------------------------------------------------*/
int nifti_image_load_bricks( nifti_image * nim , int nbricks, int * blist,
                             nifti_brick_list * NBL )
{
   int     * slist = NULL, * sindex = NULL, rv;
   znzFile   fp;

   /* we can have blist == NULL */
   if( !nim || !NBL ){
      fprintf(stderr,"** nifti_image_load_bricks, bad params (%p,%p)\n",
              nim, NBL);
      return -1;
   }

   if( blist && nbricks <= 0 ){
      if( gni_debug > 1 )
         fprintf(stderr,"-d load_bricks: received blist with nbricks = %d,"
                        "ignoring blist\n", nbricks);
      blist = NULL; /* pretend nothing was passed */
   }

   if( blist && ! valid_nifti_brick_list( nim, nbricks, blist, gni_debug>0 ) )
      return -1;

   /* for efficiency, let's read the file in order */
   if( blist && nifti_copynsort( nbricks, blist, &slist, &sindex ) != 0 )
      return -1;

   /* open the file and position the FILE pointer */
   fp = nifti_image_load_prep( nim );
   if( !fp ){
      if( gni_debug > 0 )
         fprintf(stderr,"** nifti_image_load_bricks, failed load_prep\n");
      if( blist ){ free(slist); free(sindex); }
      return -1;
   }

   /* this will flag to allocate defaults */
   if( !blist ) nbricks = 0;
   if( nifti_alloc_NBL_mem( nim, nbricks, NBL ) != 0 ){
      if( blist ){ free(slist); free(sindex); }
      znzclose(fp);
      return -1;
   }

   rv = nifti_load_NBL_bricks(nim, slist, sindex, NBL, fp);

   if( rv != 0 ){
      free_NBL( NBL );  /* failure! */
      NBL->nbricks = 0; /* repetative, but clear */
   }

   if( slist ){ free(slist); free(sindex); }

   znzclose(fp);

   return NBL->nbricks;
}


/*----------------------------------------------------------------------
 * free_NBL      - free all pointers and clear structure
 *
 * note: this does not presume to free the structure pointer
 *----------------------------------------------------------------------*/
void free_NBL( nifti_brick_list * NBL )
{
   int c;

   if( NBL->bricks ){
      for( c = 0; c < NBL->nbricks; c++ )
         if( NBL->bricks[c] ) free(NBL->bricks[c]);
      free(NBL->bricks);
      NBL->bricks = NULL;
   }

   NBL->nbricks = NBL->bsize = 0;
}


/*----------------------------------------------------------------------
 * nifti_load_NBL_bricks      - read the file data into the NBL struct
 *
 * return 0 on success, -1 on failure
 *----------------------------------------------------------------------*/
static int nifti_load_NBL_bricks( nifti_image * nim , int * slist, int * sindex,
                                  nifti_brick_list * NBL, znzFile fp )
{
   int c, rv;
   int oposn, fposn;      /* orig and current file positions */
   int prev, isrc, idest; /* previous and current sub-brick, and new index */

   oposn = znztell(fp);  /* store current file position */
   fposn = oposn;
   if( fposn < 0 ){
      fprintf(stderr,"** load bricks: ztell failed??\n");
      return -1;
   }

   /* first, handle the default case, no passed blist */
   if( !slist ){
      for( c = 0; c < NBL->nbricks; c++ ) {
         rv = nifti_read_buffer(fp, NBL->bricks[c], NBL->bsize, nim);
         if( rv != NBL->bsize ){
            fprintf(stderr,"** load bricks: cannot read brick %d from '%s'\n",
                    c, nim->iname ? nim->iname : nim->fname);
            return -1;
         }
      }
      if( gni_debug > 1 )
         fprintf(stderr,"+d read %d default bricks from file %s\n",
                 NBL->nbricks, nim->iname ? nim->iname : nim->fname );
      return 0;
   }

   if( !sindex ){
      fprintf(stderr,"** load_NBL_bricks: missing index list\n");
      return -1;
   }

   prev = -1;   /* use prev for previous sub-brick */
   for( c = 0; c < NBL->nbricks; c++ ){
       isrc = slist[c];   /* this is original brick index (c is new one) */
       idest = sindex[c]; /* this is the destination index for this data */

       /* if this sub-brick is not the previous, we must read from disk */
       if( isrc != prev ){

          /* if we are not looking at the correct sub-brick, scan forward */
          if( fposn != (oposn + isrc*NBL->bsize) ){
             fposn = oposn + isrc*NBL->bsize;
             if( znzseek(fp, fposn, SEEK_SET) < 0 ){
                fprintf(stderr,"** failed to locate brick %d in file '%s'\n",
                        isrc, nim->iname ? nim->iname : nim->fname);
                return -1;
             }
          }

          /* only 10,000 lines later and we're actually reading something! */
          rv = nifti_read_buffer(fp, NBL->bricks[idest], NBL->bsize, nim);
          if( rv != NBL->bsize ){
             fprintf(stderr,"** failed to read brick %d from file '%s'\n",
                     isrc, nim->iname ? nim->iname : nim->fname);
             return -1;
          }
          fposn += NBL->bsize;
       } else {
          /* we have already read this sub-brick, just copy the previous one */
          /* note that this works because they are sorted */
          memcpy(NBL->bricks[idest], NBL->bricks[sindex[c-1]], NBL->bsize);
       }

       prev = isrc;  /* in any case, note the now previous sub-brick */
   }

   return 0;
}


/*----------------------------------------------------------------------
 * nifti_alloc_NBL_mem      - allocate memory for bricks
 *
 * return 0 on success, -1 on failure
 *----------------------------------------------------------------------*/
static int nifti_alloc_NBL_mem(nifti_image * nim, int nbricks,
                               nifti_brick_list * nbl)
{
   int c;

   /* if nbricks is not specified, use the default */
   if( nbricks > 0 ) nbl->nbricks = nbricks;
   else              nbl->nbricks = nim->nt * nim->nu * nim->nv * nim->nw;

   nbl->bsize   = nim->nx * nim->ny * nim->nz;
   nbl->bricks  = (void **)malloc(nbl->nbricks * sizeof(void *));

   if( ! nbl->bricks ){
      fprintf(stderr,"** NANM: failed to alloc %d void ptrs\n",nbricks);
      return -1;
   }

   for( c = 0; c < nbl->nbricks; c++ ){
      nbl->bricks[c] = (void *)malloc(nbl->bsize * nim->nbyper);
      if( ! nbl->bricks[c] ){
         fprintf(stderr,"** NANM: failed to alloc %d bytes for brick %d\n",
                 nbl->bsize*nim->nbyper, c);
         /* so free and clear everything before returning */
         while( c > 0 ){
            c--;
            free(nbl->bricks[c]);
         }
         free(nbl->bricks);
         nbl->bricks = NULL;
         nbl->nbricks = nbl->bsize = 0;
         return -1;
      }
   }

   if( gni_debug > 2 )
      fprintf(stderr,"+d NANM: alloc'd %d bricks of %d bytes for NBL\n",
              nbl->nbricks, nbl->bsize*nim->nbyper);

   return 0;
}


/*----------------------------------------------------------------------
 * nifti_copynsort      - copy int list, and sort with indices
 *
 * 1. duplicate the incoming list
 * 2. create an sindex list, and init with 0..nbricks-1
 * 3. do a slow insertion sort on the small slist, along with sindex list
 * 4. check results, just to be positive
 * 
 * So slist is sorted, and sindex hold original positions.
 *
 * return 0 on success, -1 on failure
 *----------------------------------------------------------------------*/
static int nifti_copynsort(int nbricks, int * blist, int ** slist,
                           int ** sindex)
{
   int * stmp, * itmp;   /* for ease of typing/reading */
   int   c1, c2, spos, tmp;

   *slist  = (int *)malloc(nbricks * sizeof(int));
   *sindex = (int *)malloc(nbricks * sizeof(int));

   if( !*slist || !*sindex ){
      fprintf(stderr,"** NCS: failed to alloc %d ints for sorting\n",nbricks);
      if(*slist)  free(*slist);   /* maybe one succeeded */
      if(*sindex) free(*sindex);
      return -1;
   }

   /* init the lists */
   memcpy(*slist, blist, nbricks*sizeof(int));
   for( c1 = 0; c1 < nbricks; c1++ ) (*sindex)[c1] = c1;

   /* now actually sort slist */
   stmp = *slist;
   itmp = *sindex;
   for( c1 = 0; c1 < nbricks-1; c1++ ) {
      /* find smallest value, init to current */
      spos = c1;
      for( c2 = c1+1; c2 < nbricks; c2++ )
         if( stmp[c2] < stmp[spos] ) spos = c2;
      if( spos != c1 ) /* swap: fine, don't maintain sub-order, see if I care */
      {
         tmp        = stmp[c1];      /* first swap the sorting values */
         stmp[c1]   = stmp[spos];
         stmp[spos] = tmp;

         tmp        = itmp[c1];      /* then swap the index values */
         itmp[c1]   = itmp[spos];
         itmp[spos] = tmp;
      }
   }

   if( gni_debug > 2 ){
      fprintf(stderr,  "+d sorted indexing list:\n");
      fprintf(stderr,  "  orig   : ");
      for( c1 = 0; c1 < nbricks; c1++ ) fprintf(stderr,"  %d",blist[c1]);
      fprintf(stderr,"\n  new    : ");
      for( c1 = 0; c1 < nbricks; c1++ ) fprintf(stderr,"  %d",stmp[c1]);
      fprintf(stderr,"\n  indices: ");
      for( c1 = 0; c1 < nbricks; c1++ ) fprintf(stderr,"  %d",itmp[c1]);
      fputc('\n', stderr);
   }

   /* check the sort (why not?  I've got time...) */
   for( c1 = 0; c1 < nbricks-1; c1++ ){
       if( (stmp[c1] > stmp[c1+1]) || (blist[itmp[c1]] != stmp[c1]) ){
          fprintf(stderr,"** sorting screw-up, way to go, rick!\n");
          free(stmp); free(itmp); *slist = NULL; *sindex = NULL;
          return -1;
       }
   }

   if( gni_debug > 2 ) fprintf(stderr,"-d sorting is okay\n");

   return 0;
}


/*----------------------------------------------------------------------
 * valid_nifti_brick_list      - check sub-brick list for image
 *
 * return 1 if valid, 0 if not
 *----------------------------------------------------------------------*/
int valid_nifti_brick_list(nifti_image * nim , int nbricks, int * blist,
                           int disp_error)
{
   int c, nsubs;

   if( !nim ){
      if( disp_error || gni_debug > 0 )
         fprintf(stderr,"** valid_nifti_brick_list: missing nifti image\n");
      return 0;
   }

   if( nbricks <= 0 || !blist ){
      if( disp_error || gni_debug > 1 )
         fprintf(stderr,"** valid_nifti_brick_list: no brick list to check\n");
      return 0;
   }

   /* nsubs sub-brick is nt*nu*nv*nw */
   nsubs = nim->dim[4] * nim->dim[5] * nim->dim[6] * nim->dim[7];

   if( nsubs <= 0 ){
      fprintf(stderr,"** VNBL warning: bad dim list (%d,%d,%d,%d)\n",
                     nim->dim[4], nim->dim[5], nim->dim[6], nim->dim[7]);
      int_force_positive(nim->dim+4, 4);
      nsubs = nim->dim[4] * nim->dim[5] * nim->dim[6] * nim->dim[7];
   }

   for( c = 0; c < nbricks; c++ )
      if( (blist[c] < 0) || (blist[c] >= nsubs) ){
         if( disp_error || gni_debug > 1 )
            fprintf(stderr,
               "-d ** bad sub-brick chooser %d (#%d), valid range is [0,%d]\n",
               blist[c], c, nsubs-1);
         return 0;
      }

   return 1;  /* all is well */
}

/* set any non-positive values to 1 */
static int int_force_positive( int * list, int nel )
{
   int c;
   if( !list || nel < 0 ){
      if( gni_debug > 0 )
         fprintf(stderr,"** int_force_positive: bad params (%p,%d)\n",list,nel);
      return -1;
   }
   for( c = 0; c < nel; c++ )
      if( list[c] <= 0 ) list[c] = 1;
   return 0;
}
/* end of new nifti_image_read_bricks() functionality */

/*----------------------------------------------------------------------
 * display the orientation from the quaternian fields
 *
 * return < 0 if results cannot be determined, 0 if okay
 *----------------------------------------------------------------------*/
int nifti_disp_matrix_orient( char * mesg, mat44 mat )
{
   int i, j, k;

   if ( mesg ) fputs( mesg, stderr );  /* use stdout? */

   mat44_to_orientation( mat, &i,&j,&k );
   if ( i <= 0 || j <= 0 || k <= 0 ) return -1;

   /* so we have good codes */
   fprintf(stderr, "  i orientation = '%s'\n"
                   "  j orientation = '%s'\n"
                   "  k orientation = '%s'\n",
                   nifti_orientation_string(i),
                   nifti_orientation_string(j),
                   nifti_orientation_string(k) );
   return 0;
}


char *nifti_strdup(char *str)
{
  char *dup= (char *)malloc( strlen(str)+1 );
  if (dup) strcpy(dup,str);
  return dup;
}
#ifndef HAVE_STRDUP
#endif


/*---------------------------------------------------------------------------*/
/* Return a pointer to a string holding the name of a NIFTI datatype.
   Don't free() or modify this string!  It points to static storage.
-----------------------------------------------------------------------------*/

char *nifti_datatype_string( int dt )
{
   switch( dt ){
     case DT_UNKNOWN:    return "UNKNOWN"    ;
     case DT_BINARY:     return "BINARY"     ;
     case DT_INT8:       return "INT8"       ;
     case DT_UINT8:      return "UINT8"      ;
     case DT_INT16:      return "INT16"      ;
     case DT_UINT16:     return "UINT16"     ;
     case DT_INT32:      return "INT32"      ;
     case DT_UINT32:     return "UINT32"     ;
     case DT_INT64:      return "INT64"      ;
     case DT_UINT64:     return "UINT64"     ;
     case DT_FLOAT32:    return "FLOAT32"    ;
     case DT_FLOAT64:    return "FLOAT64"    ;
     case DT_FLOAT128:   return "FLOAT128"   ;
     case DT_COMPLEX64:  return "COMPLEX64"  ;
     case DT_COMPLEX128: return "COMPLEX128" ;
     case DT_COMPLEX256: return "COMPLEX256" ;
     case DT_RGB24:      return "RGB24"      ;
   }
   return "**ILLEGAL**" ;
}

/*---------------------------------------------------------------------------*/
/* Determine if the datatype code dt is an integer type (1=YES, 0=NO).
-----------------------------------------------------------------------------*/

int nifti_is_inttype( int dt )
{
   switch( dt ){
     case DT_UNKNOWN:    return 0 ;
     case DT_BINARY:     return 0 ;
     case DT_INT8:       return 1 ;
     case DT_UINT8:      return 1 ;
     case DT_INT16:      return 1 ;
     case DT_UINT16:     return 1 ;
     case DT_INT32:      return 1 ;
     case DT_UINT32:     return 1 ;
     case DT_INT64:      return 1 ;
     case DT_UINT64:     return 1 ;
     case DT_FLOAT32:    return 0 ;
     case DT_FLOAT64:    return 0 ;
     case DT_FLOAT128:   return 0 ;
     case DT_COMPLEX64:  return 0 ;
     case DT_COMPLEX128: return 0 ;
     case DT_COMPLEX256: return 0 ;
     case DT_RGB24:      return 1 ;
   }
   return 0 ;
}

/*---------------------------------------------------------------------------*/
/* Return a pointer to a string holding the name of a NIFTI units type.
   Don't free() or modify this string!  It points to static storage.
-----------------------------------------------------------------------------*/

char *nifti_units_string( int uu )
{
   switch( uu ){
     case NIFTI_UNITS_METER:  return "m" ;
     case NIFTI_UNITS_MM:     return "mm" ;
     case NIFTI_UNITS_MICRON: return "um" ;
     case NIFTI_UNITS_SEC:    return "s" ;
     case NIFTI_UNITS_MSEC:   return "ms" ;
     case NIFTI_UNITS_USEC:   return "us" ;
     case NIFTI_UNITS_HZ:     return "Hz" ;
     case NIFTI_UNITS_PPM:    return "ppm" ;
     case NIFTI_UNITS_RADS:   return "rad/s" ;
   }
   return "Unknown" ;
}

/*---------------------------------------------------------------------------*/
/* Return a pointer to a string holding the name of a NIFTI transform type.
   Don't free() or modify this string!  It points to static storage.
-----------------------------------------------------------------------------*/

char *nifti_xform_string( int xx )
{
   switch( xx ){
     case NIFTI_XFORM_SCANNER_ANAT:  return "Scanner Anat" ;
     case NIFTI_XFORM_ALIGNED_ANAT:  return "Aligned Anat" ;
     case NIFTI_XFORM_TALAIRACH:     return "Talairach" ;
     case NIFTI_XFORM_MNI_152:       return "MNI_152" ;
   }
   return "Unknown" ;
}

/*---------------------------------------------------------------------------*/
/* Return a pointer to a string holding the name of a NIFTI intent type.
   Don't free() or modify this string!  It points to static storage.
-----------------------------------------------------------------------------*/

char *nifti_intent_string( int ii )
{
   switch( ii ){
     case NIFTI_INTENT_CORREL:     return "Correlation statistic" ;
     case NIFTI_INTENT_TTEST:      return "T-statistic" ;
     case NIFTI_INTENT_FTEST:      return "F-statistic" ;
     case NIFTI_INTENT_ZSCORE:     return "Z-score"     ;
     case NIFTI_INTENT_CHISQ:      return "Chi-squared distribution" ;
     case NIFTI_INTENT_BETA:       return "Beta distribution" ;
     case NIFTI_INTENT_BINOM:      return "Binomial distribution" ;
     case NIFTI_INTENT_GAMMA:      return "Gamma distribution" ;
     case NIFTI_INTENT_POISSON:    return "Poisson distribution" ;
     case NIFTI_INTENT_NORMAL:     return "Normal distribution" ;
     case NIFTI_INTENT_FTEST_NONC: return "F-statistic noncentral" ;
     case NIFTI_INTENT_CHISQ_NONC: return "Chi-squared noncentral" ;
     case NIFTI_INTENT_LOGISTIC:   return "Logistic distribution" ;
     case NIFTI_INTENT_LAPLACE:    return "Laplace distribution" ;
     case NIFTI_INTENT_UNIFORM:    return "Uniform distribition" ;
     case NIFTI_INTENT_TTEST_NONC: return "T-statistic noncentral" ;
     case NIFTI_INTENT_WEIBULL:    return "Weibull distribution" ;
     case NIFTI_INTENT_CHI:        return "Chi distribution" ;
     case NIFTI_INTENT_INVGAUSS:   return "Inverse Gaussian distribution" ;
     case NIFTI_INTENT_EXTVAL:     return "Extreme Value distribution" ;
     case NIFTI_INTENT_PVAL:       return "P-value" ;

     case NIFTI_INTENT_LOGPVAL:    return "Log P-value" ;
     case NIFTI_INTENT_LOG10PVAL:  return "Log10 P-value" ;

     case NIFTI_INTENT_ESTIMATE:   return "Estimate" ;
     case NIFTI_INTENT_LABEL:      return "Label index" ;
     case NIFTI_INTENT_NEURONAME:  return "NeuroNames index" ;
     case NIFTI_INTENT_GENMATRIX:  return "General matrix" ;
     case NIFTI_INTENT_SYMMATRIX:  return "Symmetric matrix" ;
     case NIFTI_INTENT_DISPVECT:   return "Displacement vector" ;
     case NIFTI_INTENT_VECTOR:     return "Vector" ;
     case NIFTI_INTENT_POINTSET:   return "Pointset" ;
     case NIFTI_INTENT_TRIANGLE:   return "Triangle" ;
     case NIFTI_INTENT_QUATERNION: return "Quaternion" ;

     case NIFTI_INTENT_DIMLESS:    return "Dimensionless number" ;
   }
   return "Unknown" ;
}

/*---------------------------------------------------------------------------*/
/* Return a pointer to a string holding the name of a NIFTI slice_code
   string.  Don't free() or modify this string!  It points to static storage.
-----------------------------------------------------------------------------*/

char *nifti_slice_string( int ss )
{
   switch( ss ){
     case NIFTI_SLICE_SEQ_INC: return "sequential_increasing"  ;
     case NIFTI_SLICE_SEQ_DEC: return "sequential_decreasing"  ;
     case NIFTI_SLICE_ALT_INC: return "alternating_increasing" ;
     case NIFTI_SLICE_ALT_DEC: return "alternating_decreasing" ;
   }
   return "Unknown" ;
}

/*---------------------------------------------------------------------------*/
/* Return a pointer to a string holding the name of a NIFTI orientation
   string.  Don't free() or modify this string!  It points to static storage.
-----------------------------------------------------------------------------*/

char *nifti_orientation_string( int ii )
{
   switch( ii ){
     case NIFTI_L2R: return "Left-to-Right" ;
     case NIFTI_R2L: return "Right-to-Left" ;
     case NIFTI_P2A: return "Posterior-to-Anterior" ;
     case NIFTI_A2P: return "Anterior-to-Posterior" ;
     case NIFTI_I2S: return "Inferior-to-Superior" ;
     case NIFTI_S2I: return "Superior-to-Inferior" ;
   }
   return "Unknown" ;
}

/*--------------------------------------------------------------------------*/
/* Given a datatype code, set number of bytes per voxel and the swapsize.
   The swapsize is set to 0 if this datatype doesn't ever need swapping.
----------------------------------------------------------------------------*/

void nifti_datatype_sizes( int datatype , int *nbyper, int *swapsize )
{
   int nb=0, ss=0 ;
   switch( datatype ){
     case DT_INT8:
     case DT_UINT8:       nb =  1 ; ss =  0 ; break ;

     case DT_INT16:
     case DT_UINT16:      nb =  2 ; ss =  2 ; break ;

     case DT_RGB24:       nb =  3 ; ss =  0 ; break ;

     case DT_INT32:
     case DT_UINT32:
     case DT_FLOAT32:     nb =  4 ; ss =  4 ; break ;

     case DT_COMPLEX64:   nb =  8 ; ss =  4 ; break ;

     case DT_FLOAT64:
     case DT_INT64:
     case DT_UINT64:      nb =  8 ; ss =  8 ; break ;

     case DT_FLOAT128:    nb = 16 ; ss = 16 ; break ;

     case DT_COMPLEX128:  nb = 16 ; ss =  8 ; break ;

     case DT_COMPLEX256:  nb = 32 ; ss = 16 ; break ;
   }

   ASSIF(nbyper,nb) ; ASSIF(swapsize,ss) ; return ;
}

/*---------------------------------------------------------------------------*/
/* Given the quaternion parameters (etc.), compute a transformation matrix.
   See comments in nifti1.h for details.
     - qb,qc,qd = quaternion parameters
     - qx,qy,qz = offset parameters
     - dx,dy,dz = grid stepsizes (non-negative inputs are set to 1.0)
     - qfac     = sign of dz step (< 0 is negative; >= 0 is positive)
   If qx=qy=qz=0, dx=dy=dz=1, then the output is a rotation matrix.
   For qfac >= 0, the rotation is proper.
   For qfac <  0, the rotation is improper.
-----------------------------------------------------------------------------*/

mat44 quatern_to_mat44( float qb, float qc, float qd,
                        float qx, float qy, float qz,
                        float dx, float dy, float dz, float qfac )
{
   mat44 R ;
   double a,b=qb,c=qc,d=qd , xd,yd,zd ;

   /* last row is always [ 0 0 0 1 ] */

   R.m[3][0]=R.m[3][1]=R.m[3][2] = 0.0 ; R.m[3][3]= 1.0 ;

   /* compute a parameter from b,c,d */

   a = 1.0l - (b*b + c*c + d*d) ;
   if( a < 1.e-7l ){                   /* special case */
     a = 1.0l / sqrt(b*b+c*c+d*d) ;
     b *= a ; c *= a ; d *= a ;        /* normalize (b,c,d) vector */
     a = 0.0l ;                        /* a = 0 ==> 180 degree rotation */
   } else{
     a = sqrt(a) ;                     /* angle = 2*arccos(a) */
   }

   /* load rotation matrix, including scaling factors for voxel sizes */

   xd = (dx > 0.0) ? dx : 1.0l ;       /* make sure are positive */
   yd = (dy > 0.0) ? dy : 1.0l ;
   zd = (dz > 0.0) ? dz : 1.0l ;

   if( qfac < 0.0 ) zd = -zd ;         /* left handedness? */

   R.m[0][0] =        (a*a+b*b-c*c-d*d) * xd ;
   R.m[0][1] = 2.0l * (b*c-a*d        ) * yd ;
   R.m[0][2] = 2.0l * (b*d+a*c        ) * zd ;
   R.m[1][0] = 2.0l * (b*c+a*d        ) * xd ;
   R.m[1][1] =        (a*a+c*c-b*b-d*d) * yd ;
   R.m[1][2] = 2.0l * (c*d-a*b        ) * zd ;
   R.m[2][0] = 2.0l * (b*d-a*c        ) * xd ;
   R.m[2][1] = 2.0l * (c*d+a*b        ) * yd ;
   R.m[2][2] =        (a*a+d*d-c*c-b*b) * zd ;

   /* load offsets */

   R.m[0][3] = qx ; R.m[1][3] = qy ; R.m[2][3] = qz ;

   return R ;
}

/*---------------------------------------------------------------------------*/
/* Given the 3x4 upper corner of the matrix R, compute the quaternion
   parameters that fit it.  See comments in nifti1.h for details.
     - Any NULL pointer on input won't get assigned (e.g., if you don't want
       dx,dy,dz, just pass NULL in for those pointers).
     - If the 3 input matrix columns are NOT orthogonal, they will be
       orthogonalized prior to calculating the parameters, using
       the polar decomposition to find the orthogonal matrix closest
       to the column-normalized input matrix.
     - However, if the 3 input matrix columns are NOT orthogonal, then
       the matrix produced by quatern_to_mat44 WILL have orthogonal
       columns, so it won't be the same as the matrix input here.
       This "feature" is because the NIFTI 'qform' transform is
       deliberately not fully general -- it is intended to model a volume
       with perpendicular axes.
     - If the 3 input matrix columns are not even linearly independent,
       you'll just have to take your luck, won't you?
-----------------------------------------------------------------------------*/

void mat44_to_quatern( mat44 R ,
                       float *qb, float *qc, float *qd,
                       float *qx, float *qy, float *qz,
                       float *dx, float *dy, float *dz, float *qfac )
{
   double r11,r12,r13 , r21,r22,r23 , r31,r32,r33 ;
   double xd,yd,zd , a,b,c,d ;
   mat33 P,Q ;

   /* offset outputs are read write out of input matrix  */

   ASSIF(qx,R.m[0][3]) ; ASSIF(qy,R.m[1][3]) ; ASSIF(qz,R.m[2][3]) ;

   /* load 3x3 matrix into local variables */

   r11 = R.m[0][0] ; r12 = R.m[0][1] ; r13 = R.m[0][2] ;
   r21 = R.m[1][0] ; r22 = R.m[1][1] ; r23 = R.m[1][2] ;
   r31 = R.m[2][0] ; r32 = R.m[2][1] ; r33 = R.m[2][2] ;

   /* compute lengths of each column; these determine grid spacings  */

   xd = sqrt( r11*r11 + r21*r21 + r31*r31 ) ;
   yd = sqrt( r12*r12 + r22*r22 + r32*r32 ) ;
   zd = sqrt( r13*r13 + r23*r23 + r33*r33 ) ;

   /* if a column length is zero, patch the trouble */

   if( xd == 0.0l ){ r11 = 1.0l ; r21 = r31 = 0.0l ; xd = 1.0l ; }
   if( yd == 0.0l ){ r22 = 1.0l ; r12 = r32 = 0.0l ; yd = 1.0l ; }
   if( zd == 0.0l ){ r33 = 1.0l ; r13 = r23 = 0.0l ; zd = 1.0l ; }

   /* assign the output lengths */

   ASSIF(dx,xd) ; ASSIF(dy,yd) ; ASSIF(dz,zd) ;

   /* normalize the columns */

   r11 /= xd ; r21 /= xd ; r31 /= xd ;
   r12 /= yd ; r22 /= yd ; r32 /= yd ;
   r13 /= zd ; r23 /= zd ; r33 /= zd ;

   /* At this point, the matrix has normal columns, but we have to allow
      for the fact that the hideous user may not have given us a matrix
      with orthogonal columns.

      So, now find the orthogonal matrix closest to the current matrix.

      One reason for using the polar decomposition to get this
      orthogonal matrix, rather than just directly orthogonalizing
      the columns, is so that inputting the inverse matrix to R
      will result in the inverse orthogonal matrix at this point.
      If we just orthogonalized the columns, this wouldn't necessarily hold. */

   Q.m[0][0] = r11 ; Q.m[0][1] = r12 ; Q.m[0][2] = r13 ; /* load Q */
   Q.m[1][0] = r21 ; Q.m[1][1] = r22 ; Q.m[1][2] = r23 ;
   Q.m[2][0] = r31 ; Q.m[2][1] = r32 ; Q.m[2][2] = r33 ;

   P = mat33_polar(Q) ;  /* P is orthog matrix closest to Q */

   r11 = P.m[0][0] ; r12 = P.m[0][1] ; r13 = P.m[0][2] ; /* unload */
   r21 = P.m[1][0] ; r22 = P.m[1][1] ; r23 = P.m[1][2] ;
   r31 = P.m[2][0] ; r32 = P.m[2][1] ; r33 = P.m[2][2] ;

   /*                            [ r11 r12 r13 ]               */
   /* at this point, the matrix  [ r21 r22 r23 ] is orthogonal */
   /*                            [ r31 r32 r33 ]               */

   /* compute the determinant to determine if it is proper */

   zd = r11*r22*r33-r11*r32*r23-r21*r12*r33
       +r21*r32*r13+r31*r12*r23-r31*r22*r13 ;  /* should be -1 or 1 */

   if( zd > 0 ){             /* proper */
     ASSIF(qfac,1.0) ;
   } else {                  /* improper ==> flip 3rd column */
     ASSIF(qfac,-1.0) ;
     r13 = -r13 ; r23 = -r23 ; r33 = -r33 ;
   }

   /* now, compute quaternion parameters */

   a = r11 + r22 + r33 + 1.0l ;

   if( a > 0.5l ){                /* simplest case */
     a = 0.5l * sqrt(a) ;
     b = 0.25l * (r32-r23) / a ;
     c = 0.25l * (r13-r31) / a ;
     d = 0.25l * (r21-r12) / a ;
   } else {                       /* trickier case */
     xd = 1.0 + r11 - (r22+r33) ;  /* 4*b*b */
     yd = 1.0 + r22 - (r11+r33) ;  /* 4*c*c */
     zd = 1.0 + r33 - (r11+r22) ;  /* 4*d*d */
     if( xd > 1.0 ){
       b = 0.5l * sqrt(xd) ;
       c = 0.25l* (r12+r21) / b ;
       d = 0.25l* (r13+r31) / b ;
       a = 0.25l* (r32-r23) / b ;
     } else if( yd > 1.0 ){
       c = 0.5l * sqrt(yd) ;
       b = 0.25l* (r12+r21) / c ;
       d = 0.25l* (r23+r32) / c ;
       a = 0.25l* (r13-r31) / c ;
     } else {
       d = 0.5l * sqrt(zd) ;
       b = 0.25l* (r13+r31) / d ;
       c = 0.25l* (r23+r32) / d ;
       a = 0.25l* (r21-r12) / d ;
     }
     if( a < 0.0l ){ b=-b ; c=-c ; d=-d; a=-a; }
   }

   ASSIF(qb,b) ; ASSIF(qc,c) ; ASSIF(qd,d) ;
   return ;
}

/*---------------------------------------------------------------------------*/
/* Compute the inverse of a bordered 4x4 matrix.
   Some numerical code fragments were generated by Maple 8.
   If a singular matrix is input, the output matrix will be all zero.
   You can check for this by examining the [3][3] element, which will
   be 1.0 for the normal case and 0.0 for the bad case.
-----------------------------------------------------------------------------*/

mat44 mat44_inverse( mat44 R )
{
   double r11,r12,r13,r21,r22,r23,r31,r32,r33,v1,v2,v3 , deti ;
   mat44 Q ;
                                                       /** INPUT MATRIX IS: **/
   r11 = R.m[0][0]; r12 = R.m[0][1]; r13 = R.m[0][2];  /* [ r11 r12 r13 v1 ] */
   r21 = R.m[1][0]; r22 = R.m[1][1]; r23 = R.m[1][2];  /* [ r21 r22 r23 v2 ] */
   r31 = R.m[2][0]; r32 = R.m[2][1]; r33 = R.m[2][2];  /* [ r31 r32 r33 v3 ] */
   v1  = R.m[0][3]; v2  = R.m[1][3]; v3  = R.m[2][3];  /* [  0   0   0   1 ] */

   deti = r11*r22*r33-r11*r32*r23-r21*r12*r33
         +r21*r32*r13+r31*r12*r23-r31*r22*r13 ;

   if( deti != 0.0l ) deti = 1.0l / deti ;

   Q.m[0][0] = deti*( r22*r33-r32*r23) ;
   Q.m[0][1] = deti*(-r12*r33+r32*r13) ;
   Q.m[0][2] = deti*( r12*r23-r22*r13) ;
   Q.m[0][3] = deti*(-r12*r23*v3+r12*v2*r33+r22*r13*v3
                     -r22*v1*r33-r32*r13*v2+r32*v1*r23) ;

   Q.m[1][0] = deti*(-r21*r33+r31*r23) ;
   Q.m[1][1] = deti*( r11*r33-r31*r13) ;
   Q.m[1][2] = deti*(-r11*r23+r21*r13) ;
   Q.m[1][3] = deti*( r11*r23*v3-r11*v2*r33-r21*r13*v3
                     +r21*v1*r33+r31*r13*v2-r31*v1*r23) ;

   Q.m[2][0] = deti*( r21*r32-r31*r22) ;
   Q.m[2][1] = deti*(-r11*r32+r31*r12) ;
   Q.m[2][2] = deti*( r11*r22-r21*r12) ;
   Q.m[2][3] = deti*(-r11*r22*v3+r11*r32*v2+r21*r12*v3
                     -r21*r32*v1-r31*r12*v2+r31*r22*v1) ;

   Q.m[3][0] = Q.m[3][1] = Q.m[3][2] = 0.0l ;
   Q.m[3][3] = (deti == 0.0l) ? 0.0l : 1.0l ; /* failure flag if deti == 0 */

   return Q ;
}

/*---------------------------------------------------------------------------*/
/* Input 9 floats and make an orthgonal mat44 out of them.
   Each row is normalized, then mat33_polar() is used to orthogonalize them.
   If row #3 (r31,r32,r33) is input as zero, then it will be taken to be
   the cross product of rows #1 and #2.

   This function can be used to create a rotation matrix for transforming
   an oblique volume to anatomical coordinates.  For this application:
    - row #1 (r11,r12,r13) is the direction vector along the image i-axis
    - row #2 (r21,r22,r23) is the direction vector along the image j-axis
    - row #3 (r31,r32,r33) is the direction vector along the slice direction
      (if available; otherwise enter it as 0's)
   The first 2 rows can be taken from the DICOM attribute (0020,0037)
   "Image Orientation (Patient)".

   After forming the rotation matrix, the complete affine transformation from
   (i,j,k) grid indexes to (x,y,z) spatial coordinates can be computed by
   multiplying each column by the appropriate grid spacing:
    - column #1 (R.m[0][0],R.m[1][0],R.m[2][0]) by delta-x
    - column #2 (R.m[0][1],R.m[1][1],R.m[2][1]) by delta-y
    - column #3 (R.m[0][2],R.m[1][2],R.m[2][2]) by delta-z
   And by placing the center (x,y,z) coordinates of voxel (0,0,0) into
   the column #4 (R.m[0][3],R.m[1][3],R.m[2][3]).
-----------------------------------------------------------------------------*/

mat44 make_orthog_mat44( float r11, float r12, float r13 ,
                         float r21, float r22, float r23 ,
                         float r31, float r32, float r33  )
{
   mat44 R ;
   mat33 Q , P ;
   double val ;

   R.m[3][0] = R.m[3][1] = R.m[3][2] = 0.0l ; R.m[3][3] = 1.0l ;

   Q.m[0][0] = r11 ; Q.m[0][1] = r12 ; Q.m[0][2] = r13 ; /* load Q */
   Q.m[1][0] = r21 ; Q.m[1][1] = r22 ; Q.m[1][2] = r23 ;
   Q.m[2][0] = r31 ; Q.m[2][1] = r32 ; Q.m[2][2] = r33 ;

   /* normalize row 1 */

   val = Q.m[0][0]*Q.m[0][0] + Q.m[0][1]*Q.m[0][1] + Q.m[0][2]*Q.m[0][2] ;
   if( val > 0.0l ){
     val = 1.0l / sqrt(val) ;
     Q.m[0][0] *= val ; Q.m[0][1] *= val ; Q.m[0][2] *= val ;
   } else {
     Q.m[0][0] = 1.0l ; Q.m[0][1] = 0.0l ; Q.m[0][2] = 0.0l ;
   }

   /* normalize row 2 */

   val = Q.m[1][0]*Q.m[1][0] + Q.m[1][1]*Q.m[1][1] + Q.m[1][2]*Q.m[1][2] ;
   if( val > 0.0l ){
     val = 1.0l / sqrt(val) ;
     Q.m[1][0] *= val ; Q.m[1][1] *= val ; Q.m[1][2] *= val ;
   } else {
     Q.m[1][0] = 0.0l ; Q.m[1][1] = 1.0l ; Q.m[1][2] = 0.0l ;
   }

   /* normalize row 3 */

   val = Q.m[2][0]*Q.m[2][0] + Q.m[2][1]*Q.m[2][1] + Q.m[2][2]*Q.m[2][2] ;
   if( val > 0.0l ){
     val = 1.0l / sqrt(val) ;
     Q.m[2][0] *= val ; Q.m[2][1] *= val ; Q.m[2][2] *= val ;
   } else {
     Q.m[2][0] = Q.m[0][1]*Q.m[1][2] - Q.m[0][2]*Q.m[1][1] ;  /* cross */
     Q.m[2][1] = Q.m[0][2]*Q.m[1][0] - Q.m[0][0]*Q.m[1][2] ;  /* product */
     Q.m[2][2] = Q.m[0][0]*Q.m[1][1] - Q.m[0][1]*Q.m[1][0] ;
   }

   P = mat33_polar(Q) ;  /* P is orthog matrix closest to Q */

   R.m[0][0] = P.m[0][0] ; R.m[0][1] = P.m[0][1] ; R.m[0][2] = P.m[0][2] ;
   R.m[1][0] = P.m[1][0] ; R.m[1][1] = P.m[1][1] ; R.m[1][2] = P.m[1][2] ;
   R.m[2][0] = P.m[2][0] ; R.m[2][1] = P.m[2][1] ; R.m[2][2] = P.m[2][2] ;

   R.m[0][3] = R.m[1][3] = R.m[2][3] = 0.0 ; return R ;
}

/*---------------------------------------------------------------------------*/

mat33 mat33_inverse( mat33 R )   /* inverse of 3x3 matrix */
{
   double r11,r12,r13,r21,r22,r23,r31,r32,r33 , deti ;
   mat33 Q ;
                                                       /** INPUT MATRIX: **/
   r11 = R.m[0][0]; r12 = R.m[0][1]; r13 = R.m[0][2];  /* [ r11 r12 r13 ] */
   r21 = R.m[1][0]; r22 = R.m[1][1]; r23 = R.m[1][2];  /* [ r21 r22 r23 ] */
   r31 = R.m[2][0]; r32 = R.m[2][1]; r33 = R.m[2][2];  /* [ r31 r32 r33 ] */

   deti = r11*r22*r33-r11*r32*r23-r21*r12*r33
         +r21*r32*r13+r31*r12*r23-r31*r22*r13 ;

   if( deti != 0.0l ) deti = 1.0l / deti ;

   Q.m[0][0] = deti*( r22*r33-r32*r23) ;
   Q.m[0][1] = deti*(-r12*r33+r32*r13) ;
   Q.m[0][2] = deti*( r12*r23-r22*r13) ;

   Q.m[1][0] = deti*(-r21*r33+r31*r23) ;
   Q.m[1][1] = deti*( r11*r33-r31*r13) ;
   Q.m[1][2] = deti*(-r11*r23+r21*r13) ;

   Q.m[2][0] = deti*( r21*r32-r31*r22) ;
   Q.m[2][1] = deti*(-r11*r32+r31*r12) ;
   Q.m[2][2] = deti*( r11*r22-r21*r12) ;

   return Q ;
}

/*---------------------------------------------------------------------------*/

float mat33_determ( mat33 R )   /* determinant of 3x3 matrix */
{
   double r11,r12,r13,r21,r22,r23,r31,r32,r33 ;
                                                       /** INPUT MATRIX: **/
   r11 = R.m[0][0]; r12 = R.m[0][1]; r13 = R.m[0][2];  /* [ r11 r12 r13 ] */
   r21 = R.m[1][0]; r22 = R.m[1][1]; r23 = R.m[1][2];  /* [ r21 r22 r23 ] */
   r31 = R.m[2][0]; r32 = R.m[2][1]; r33 = R.m[2][2];  /* [ r31 r32 r33 ] */

   return r11*r22*r33-r11*r32*r23-r21*r12*r33
         +r21*r32*r13+r31*r12*r23-r31*r22*r13 ;
}

/*---------------------------------------------------------------------------*/

float mat33_rownorm( mat33 A )  /* max row norm of 3x3 matrix */
{
   float r1,r2,r3 ;

   r1 = fabs(A.m[0][0])+fabs(A.m[0][1])+fabs(A.m[0][2]) ;
   r2 = fabs(A.m[1][0])+fabs(A.m[1][1])+fabs(A.m[1][2]) ;
   r3 = fabs(A.m[2][0])+fabs(A.m[2][1])+fabs(A.m[2][2]) ;
   if( r1 < r2 ) r1 = r2 ;
   if( r1 < r3 ) r1 = r3 ;
   return r1 ;
}

/*---------------------------------------------------------------------------*/

float mat33_colnorm( mat33 A )  /* max column norm of 3x3 matrix */
{
   float r1,r2,r3 ;

   r1 = fabs(A.m[0][0])+fabs(A.m[1][0])+fabs(A.m[2][0]) ;
   r2 = fabs(A.m[0][1])+fabs(A.m[1][1])+fabs(A.m[2][1]) ;
   r3 = fabs(A.m[0][2])+fabs(A.m[1][2])+fabs(A.m[2][2]) ;
   if( r1 < r2 ) r1 = r2 ;
   if( r1 < r3 ) r1 = r3 ;
   return r1 ;
}

/*---------------------------------------------------------------------------*/

mat33 mat33_mul( mat33 A , mat33 B )  /* multiply 2 3x3 matrices */
{
   mat33 C ; int i,j ;
   for( i=0 ; i < 3 ; i++ )
    for( j=0 ; j < 3 ; j++ )
      C.m[i][j] =  A.m[i][0] * B.m[0][j]
                 + A.m[i][1] * B.m[1][j]
                 + A.m[i][2] * B.m[2][j] ;
   return C ;
}

/*---------------------------------------------------------------------------*/
/* Polar decomposition of a 3x3 matrix: finds the closest orthogonal matrix
   to input A (in both the Frobenius and L2 norms).  Algorithm is that from
   NJ Higham, SIAM J Sci Stat Comput, 7:1160-1174.
-----------------------------------------------------------------------------*/

mat33 mat33_polar( mat33 A )
{
   mat33 X , Y , Z ;
   float alp,bet,gam,gmi , dif=1.0 ;
   int k=0 ;

   X = A ;

   /* force matrix to be nonsingular */

   gam = mat33_determ(X) ;
   while( gam == 0.0 ){        /* perturb matrix */
     gam = 0.00001 * ( 0.001 + mat33_rownorm(X) ) ;
     X.m[0][0] += gam ; X.m[1][1] += gam ; X.m[2][2] += gam ;
     gam = mat33_determ(X) ;
   }

   while(1){
     Y = mat33_inverse(X) ;
     if( dif > 0.3 ){     /* far from convergence */
       alp = sqrt( mat33_rownorm(X) * mat33_colnorm(X) ) ;
       bet = sqrt( mat33_rownorm(Y) * mat33_colnorm(Y) ) ;
       gam = sqrt( bet / alp ) ;
       gmi = 1.0 / gam ;
     } else {
       gam = gmi = 1.0 ;  /* close to convergence */
     }
     Z.m[0][0] = 0.5 * ( gam*X.m[0][0] + gmi*Y.m[0][0] ) ;
     Z.m[0][1] = 0.5 * ( gam*X.m[0][1] + gmi*Y.m[1][0] ) ;
     Z.m[0][2] = 0.5 * ( gam*X.m[0][2] + gmi*Y.m[2][0] ) ;
     Z.m[1][0] = 0.5 * ( gam*X.m[1][0] + gmi*Y.m[0][1] ) ;
     Z.m[1][1] = 0.5 * ( gam*X.m[1][1] + gmi*Y.m[1][1] ) ;
     Z.m[1][2] = 0.5 * ( gam*X.m[1][2] + gmi*Y.m[2][1] ) ;
     Z.m[2][0] = 0.5 * ( gam*X.m[2][0] + gmi*Y.m[0][2] ) ;
     Z.m[2][1] = 0.5 * ( gam*X.m[2][1] + gmi*Y.m[1][2] ) ;
     Z.m[2][2] = 0.5 * ( gam*X.m[2][2] + gmi*Y.m[2][2] ) ;

     dif = fabs(Z.m[0][0]-X.m[0][0])+fabs(Z.m[0][1]-X.m[0][1])
          +fabs(Z.m[0][2]-X.m[0][2])+fabs(Z.m[1][0]-X.m[1][0])
          +fabs(Z.m[1][1]-X.m[1][1])+fabs(Z.m[1][2]-X.m[1][2])
          +fabs(Z.m[2][0]-X.m[2][0])+fabs(Z.m[2][1]-X.m[2][1])
          +fabs(Z.m[2][2]-X.m[2][2])                          ;

     k = k+1 ;
     if( k > 100 || dif < 3.e-6 ) break ;  /* convergence or exhaustion */
     X = Z ;
   }

   return Z ;
}

/*---------------------------------------------------------------------------*/
/* Input:  4x4 matrix that transforms (i,j,k) indexes to (x,y,z) coordinates,
           where +x=Right, +y=Anterior, +z=Superior.
           (Only the upper-left 3x3 corner of R is used herein.)
   Output: 3 orientation codes that correspond to the closest "standard"
           anatomical orientation of the (i,j,k) axes.
   Method: Find which permutation of (x,y,z) has the smallest angle to the
           (i,j,k) axes directions, which are the columns of the R matrix.
   Errors: The codes returned will be zero.

   For example, an axial volume might get return values of
     *icod = NIFTI_R2L   (i axis is mostly Right to Left)
     *jcod = NIFTI_P2A   (j axis is mostly Posterior to Anterior)
     *kcod = NIFTI_I2S   (k axis is mostly Inferior to Superior)
-----------------------------------------------------------------------------*/

void mat44_to_orientation( mat44 R , int *icod, int *jcod, int *kcod )
{
   float xi,xj,xk , yi,yj,yk , zi,zj,zk , val,detQ,detP ;
   mat33 P , Q , M ;
   int i,j,k=0,p,q,r , ibest,jbest,kbest,pbest,qbest,rbest ;
   float vbest ;

   if( icod == NULL || jcod == NULL || kcod == NULL ) return ; /* bad */

   *icod = *jcod = *kcod = 0 ; /* error returns, if sh*t happens */

   /* load column vectors for each (i,j,k) direction from matrix */

   /*-- i axis --*/ /*-- j axis --*/ /*-- k axis --*/

   xi = R.m[0][0] ; xj = R.m[0][1] ; xk = R.m[0][2] ;
   yi = R.m[1][0] ; yj = R.m[1][1] ; yk = R.m[1][2] ;
   zi = R.m[2][0] ; zj = R.m[2][1] ; zk = R.m[2][2] ;

   /* normalize column vectors to get unit vectors along each ijk-axis */

   /* normalize i axis */

   val = sqrt( xi*xi + yi*yi + zi*zi ) ;
   if( val == 0.0 ) return ;                 /* stupid input */
   xi /= val ; yi /= val ; zi /= val ;

   /* normalize j axis */

   val = sqrt( xj*xj + yj*yj + zj*zj ) ;
   if( val == 0.0 ) return ;                 /* stupid input */
   xj /= val ; yj /= val ; zj /= val ;

   /* orthogonalize j axis to i axis, if needed */

   val = xi*xj + yi*yj + zi*zj ;    /* dot product between i and j */
   if( fabs(val) > 1.e-4 ){
     xj -= val*xi ; yj -= val*yi ; zj -= val*zi ;
     val = sqrt( xj*xj + yj*yj + zj*zj ) ;  /* must renormalize */
     if( val == 0.0 ) return ;              /* j was parallel to i? */
     xj /= val ; yj /= val ; zj /= val ;
   }

   /* normalize k axis; if it is zero, make it the cross product i x j */

   val = sqrt( xk*xk + yk*yk + zk*zk ) ;
   if( val == 0.0 ){ xk = yi*zj-zi*yj; yk = zi*xj-zj*xi ; zk=xi*yj-yi*xj ; }
   else            { xk /= val ; yk /= val ; zk /= val ; }

   /* orthogonalize k to i */

   val = xi*xk + yi*yk + zi*zk ;    /* dot product between i and k */
   if( fabs(val) > 1.e-4 ){
     xk -= val*xi ; yk -= val*yi ; zk -= val*zi ;
     val = sqrt( xk*xk + yk*yk + zk*zk ) ;
     if( val == 0.0 ) return ;      /* bad */
     xk /= val ; yk /= val ; zk /= val ;
   }

   /* orthogonalize k to j */

   val = xj*xk + yj*yk + zj*zk ;    /* dot product between j and k */
   if( fabs(val) > 1.e-4 ){
     xk -= val*xj ; yk -= val*yj ; zk -= val*zj ;
     val = sqrt( xk*xk + yk*yk + zk*zk ) ;
     if( val == 0.0 ) return ;      /* bad */
     xk /= val ; yk /= val ; zk /= val ;
   }

   Q.m[0][0] = xi ; Q.m[0][1] = xj ; Q.m[0][2] = xk ;
   Q.m[1][0] = yi ; Q.m[1][1] = yj ; Q.m[1][2] = yk ;
   Q.m[2][0] = zi ; Q.m[2][1] = zj ; Q.m[2][2] = zk ;

   /* at this point, Q is the rotation matrix from the (i,j,k) to (x,y,z) axes */

   detQ = mat33_determ( Q ) ;
   if( detQ == 0.0 ) return ; /* shouldn't happen unless user is a DUFIS */

   /* Build and test all possible +1/-1 coordinate permutation matrices P;
      then find the P such that the rotation matrix M=PQ is closest to the
      identity, in the sense of M having the smallest total rotation angle. */

   /* Despite the formidable looking 6 nested loops, there are
      only 3*3*3*2*2*2 = 216 passes, which will run very quickly. */

   vbest = -666.0 ; ibest=pbest=qbest=rbest=1 ; jbest=2 ; kbest=3 ;
   for( i=1 ; i <= 3 ; i++ ){     /* i = column number to use for row #1 */
    for( j=1 ; j <= 3 ; j++ ){    /* j = column number to use for row #2 */
     if( i == j ) continue ;
      for( k=1 ; k <= 3 ; k++ ){  /* k = column number to use for row #3 */
       if( i == k || j == k ) continue ;
       P.m[0][0] = P.m[0][1] = P.m[0][2] =
        P.m[1][0] = P.m[1][1] = P.m[1][2] =
         P.m[2][0] = P.m[2][1] = P.m[2][2] = 0.0 ;
       for( p=-1 ; p <= 1 ; p+=2 ){    /* p,q,r are -1 or +1      */
        for( q=-1 ; q <= 1 ; q+=2 ){   /* and go into rows #1,2,3 */
         for( r=-1 ; r <= 1 ; r+=2 ){
           P.m[0][i-1] = p ; P.m[1][j-1] = q ; P.m[2][k-1] = r ;
           detP = mat33_determ(P) ;             /* sign of permutation */
           if( detP * detQ <= 0.0 ) continue ;  /* doesn't match sign of Q */
           M = mat33_mul(P,Q) ;

           /* angle of M rotation = 2.0*acos(0.5*sqrt(1.0+trace(M)))       */
           /* we want largest trace(M) == smallest angle == M nearest to I */

           val = M.m[0][0] + M.m[1][1] + M.m[2][2] ; /* trace */
           if( val > vbest ){
             vbest = val ;
             ibest = i ; jbest = j ; kbest = k ;
             pbest = p ; qbest = q ; rbest = r ;
           }
   }}}}}}

   /* At this point ibest is 1 or 2 or 3; pbest is -1 or +1; etc.

      The matrix P that corresponds is the best permutation approximation
      to Q-inverse; that is, P (approximately) takes (x,y,z) coordinates
      to the (i,j,k) axes.

      For example, the first row of P (which contains pbest in column ibest)
      determines the way the i axis points relative to the anatomical
      (x,y,z) axes.  If ibest is 2, then the i axis is along the y axis,
      which is direction P2A (if pbest > 0) or A2P (if pbest < 0).

      So, using ibest and pbest, we can assign the output code for
      the i axis.  Mutatis mutandis for the j and k axes, of course. */

   switch( ibest*pbest ){
     case  1: i = NIFTI_L2R ; break ;
     case -1: i = NIFTI_R2L ; break ;
     case  2: i = NIFTI_P2A ; break ;
     case -2: i = NIFTI_A2P ; break ;
     case  3: i = NIFTI_I2S ; break ;
     case -3: i = NIFTI_S2I ; break ;
   }

   switch( jbest*qbest ){
     case  1: j = NIFTI_L2R ; break ;
     case -1: j = NIFTI_R2L ; break ;
     case  2: j = NIFTI_P2A ; break ;
     case -2: j = NIFTI_A2P ; break ;
     case  3: j = NIFTI_I2S ; break ;
     case -3: j = NIFTI_S2I ; break ;
   }

   switch( kbest*rbest ){
     case  1: k = NIFTI_L2R ; break ;
     case -1: k = NIFTI_R2L ; break ;
     case  2: k = NIFTI_P2A ; break ;
     case -2: k = NIFTI_A2P ; break ;
     case  3: k = NIFTI_I2S ; break ;
     case -3: k = NIFTI_S2I ; break ;
   }

   *icod = i ; *jcod = j ; *kcod = k ; return ;
}

/*---------------------------------------------------------------------------*/
/* Routines to swap byte arrays in various ways:
    -  2 at a time:  ab               -> ba               [short]
    -  4 at a time:  abcd             -> dcba             [int, float]
    -  8 at a time:  abcdDCBA         -> ABCDdcba         [long long, double]
    - 16 at a time:  abcdefghHGFEDCBA -> ABCDEFGHhgfedcba [long double]
-----------------------------------------------------------------------------*/

typedef struct { unsigned char a,b ; } twobytes ;

void swap_2bytes( int n , void *ar )    /* 2 bytes at a time */
{
   register int ii ;
   register twobytes *tb = (twobytes *)ar ;
   register unsigned char tt ;

   for( ii=0 ; ii < n ; ii++ ){
     tt = tb[ii].a ; tb[ii].a = tb[ii].b ; tb[ii].b = tt ;
   }
   return ;
}

/*---------------------------------------------------------------------------*/

typedef struct { unsigned char a,b,c,d ; } fourbytes ;

void swap_4bytes( int n , void *ar )    /* 4 bytes at a time */
{
   register int ii ;
   register fourbytes *tb = (fourbytes *)ar ;
   register unsigned char tt ;

   for( ii=0 ; ii < n ; ii++ ){
     tt = tb[ii].a ; tb[ii].a = tb[ii].d ; tb[ii].d = tt ;
     tt = tb[ii].b ; tb[ii].b = tb[ii].c ; tb[ii].c = tt ;
   }
   return ;
}

/*---------------------------------------------------------------------------*/

typedef struct { unsigned char a,b,c,d , D,C,B,A ; } eightbytes ;

void swap_8bytes( int n , void *ar )    /* 8 bytes at a time */
{
   register int ii ;
   register eightbytes *tb = (eightbytes *)ar ;
   register unsigned char tt ;

   for( ii=0 ; ii < n ; ii++ ){
     tt = tb[ii].a ; tb[ii].a = tb[ii].A ; tb[ii].A = tt ;
     tt = tb[ii].b ; tb[ii].b = tb[ii].B ; tb[ii].B = tt ;
     tt = tb[ii].c ; tb[ii].c = tb[ii].C ; tb[ii].C = tt ;
     tt = tb[ii].d ; tb[ii].d = tb[ii].D ; tb[ii].D = tt ;
   }
   return ;
}

/*---------------------------------------------------------------------------*/

typedef struct { unsigned char a,b,c,d,e,f,g,h ,
                               H,G,F,E,D,C,B,A  ; } sixteenbytes ;

void swap_16bytes( int n , void *ar )    /* 16 bytes at a time */
{
   register int ii ;
   register sixteenbytes *tb = (sixteenbytes *)ar ;
   register unsigned char tt ;

   for( ii=0 ; ii < n ; ii++ ){
     tt = tb[ii].a ; tb[ii].a = tb[ii].A ; tb[ii].A = tt ;
     tt = tb[ii].b ; tb[ii].b = tb[ii].B ; tb[ii].B = tt ;
     tt = tb[ii].c ; tb[ii].c = tb[ii].C ; tb[ii].C = tt ;
     tt = tb[ii].d ; tb[ii].d = tb[ii].D ; tb[ii].D = tt ;

     tt = tb[ii].e ; tb[ii].e = tb[ii].E ; tb[ii].E = tt ;
     tt = tb[ii].f ; tb[ii].f = tb[ii].F ; tb[ii].F = tt ;
     tt = tb[ii].g ; tb[ii].g = tb[ii].G ; tb[ii].G = tt ;
     tt = tb[ii].h ; tb[ii].h = tb[ii].H ; tb[ii].H = tt ;
   }
   return ;
}

/*---------------------------------------------------------------------------*/

void swap_Nbytes( int n , int siz , void *ar )  /* subsuming case */
{
   switch( siz ){
     case 2:  swap_2bytes ( n , ar ) ; break ;
     case 4:  swap_4bytes ( n , ar ) ; break ;
     case 8:  swap_8bytes ( n , ar ) ; break ;
     case 16: swap_16bytes( n , ar ) ; break ;
   }
   return ;
}

/*---------------------------------------------------------------*/
/* Byte swap NIFTI-1 file header in various places and ways.
   If is_nifti is nonzero, will also swap the NIFTI-specific
   components of the header; otherwise, only the components
   common to NIFTI and ANALYZE will be swapped.
---------------------------------------------------------------- */

void swap_nifti_header( struct nifti_1_header *h , int is_nifti )
{

#if 0                /* ANALYZE fields not used by this software */
   swap_4(h->sizeof_hdr) ;
   swap_4(h->extents) ;
   swap_2(h->session_error) ;
   swap_4(h->compressed) ;
   swap_4(h->glmax) ; swap_4(h->glmin) ;
#endif

   /* this stuff is always present, for ANALYZE and NIFTI */

   swap_4(h->sizeof_hdr) ;
   swap_2bytes( 8 , h->dim ) ;
   swap_4bytes( 8 , h->pixdim ) ;

   swap_2(h->datatype) ;
   swap_2(h->bitpix) ;

   swap_4(h->vox_offset); swap_4(h->cal_max); swap_4(h->cal_min);

   /* this stuff is NIFTI specific */

   if( is_nifti ){
     swap_4(h->intent_p1); swap_4(h->intent_p2); swap_4(h->intent_p3);
     swap_2(h->intent_code);

     swap_2(h->slice_start);    swap_2(h->slice_end);
     swap_4(h->scl_slope);      swap_4(h->scl_inter);
     swap_4(h->slice_duration); swap_4(h->toffset);

     swap_2(h->qform_code); swap_2(h->sform_code);
     swap_4(h->quatern_b); swap_4(h->quatern_c); swap_4(h->quatern_d);
     swap_4(h->qoffset_x); swap_4(h->qoffset_y); swap_4(h->qoffset_z);
     swap_4bytes(4,h->srow_x);
     swap_4bytes(4,h->srow_y);
     swap_4bytes(4,h->srow_z);
   }
   return ;
}


#define USE_STAT
#ifdef  USE_STAT
/*---------------------------------------------------------------------------*/
/* Return the file length (0 if file not found or has no contents).
   This is a Unix-specific function, since it uses stat().
-----------------------------------------------------------------------------*/
#include <sys/types.h>
#include <sys/stat.h>

/* change both to int: -1 means no file or error         20 Dec 2004 [rickr] */
int get_filesize( char *pathname )
{
   struct stat buf ; int ii ;

   if( pathname == NULL || *pathname == '\0' ) return -1 ;
   ii = stat( pathname , &buf ); if( ii != 0 ) return -1 ;
   return (unsigned int)buf.st_size ;
}

#else  /*---------- non-Unix version of the above, less efficient -----------*/

int get_filesize( char *pathname )
{
   znzFile fp ; int len ;

   if( pathname == NULL || *pathname == '\0' ) return -1 ;
   fp = znzopen(pathname,"rb",0); if( znz_isnull(fp) ) return -1 ;
   znzseek(fp,0L,SEEK_END) ; len = znztell(fp) ;
   znzclose(fp) ; return len ;
}

#endif /* USE_STAT */


size_t nifti_get_volsize(nifti_image *nim)
{
   return (size_t)(nim->nbyper) * (size_t)(nim->nvox) ; /* total bytes */
}


/*--------------------------------------------------------------------------*/
/* Support functions for filenames in read and write - allows for gzipped
   files
*/


int nifti_fileexists(char* fname)
{
   znzFile fp;
   fp = znzopen( fname , "rb" , 1 ) ;
   if( !znz_isnull(fp) )  { znzclose(fp);  return 1; }
   return 0;
}

int nifti_validfilename(char* fname)
{
   char * ext;

   /** check input file(s) for sanity **/
   if( fname == NULL || *fname == '\0' ){
      if ( gni_debug > 1 )
         fprintf(stderr,"-- empty filename in nifti_validfilename()\n");
      return 0;
   }

   ext = nifti_find_file_extension((char *)fname);

   if ( ext && ext == fname ) {   /* then no filename prefix */
      if ( gni_debug > 0 )
         fprintf(stderr,"-- no prefix for filename '%s'\n", fname);
      return 0;
   }

   return 1;
}

char * nifti_find_file_extension( char * name )
{
   char * ext;
   int    len;

   if ( ! name ) return NULL;

   len = strlen(name);
   if ( len < 4 ) return NULL;

   ext = name + len - 4;

   if ( (strcmp(ext, ".hdr") == 0) || (strcmp(ext, ".img") == 0) ||
        (strcmp(ext, ".nia") == 0) || (strcmp(ext, ".nii") == 0) )
      return ext;

#ifdef HAVE_ZLIB
   if ( len < 7 ) return NULL;

   ext = name + len - 7;

   if ( (strcmp(ext, ".hdr.gz") == 0) || (strcmp(ext, ".img.gz") == 0) ||
        (strcmp(ext, ".nia.gz") == 0) || (strcmp(ext, ".nii.gz") == 0) )
      return ext;
#endif

   return NULL;
}

int nifti_is_gzfile(char* fname)
{
  /* return true if the filename ends with .gz */
  if (fname == NULL) { return 0; }
#ifdef HAVE_ZLIB
  if (strcmp(fname + strlen(fname) - 3,".gz")==0) { return 1; }
#endif
  return 0;
}


char * nifti_makebasename(char* fname)
{
   /*
      NB: it allocates memory for basename which should be freed
	when no longer required
   */
   char *basename, *ext;

   basename=nifti_strdup(fname);

   ext = nifti_find_file_extension(basename);
   if ( ext ) *ext = '\0';  /* clear out extension */
   
   return basename;  /* in either case */
}

void nifti_set_debug_level( int level )
{
    gni_debug = level;
}


/*----------------------------------------------------------------------
 * returns filename of header on success and NULL if no appropriate file
 *   could be found 
 * NB: it allocates memory for hdrname which should be freed
 *   when no longer required
 ----------------------------------------------------------------------*/
char * nifti_findhdrname(char* fname)
{
   char *basename, *hdrname, *ext;
   char  elist[2][5] = { ".hdr", ".nii" };
   int   efirst;

   /** check input file(s) for sanity **/
   if( !nifti_validfilename(fname) ) return NULL;

   basename = nifti_makebasename(fname);
   if( !basename ) return NULL;   /* only on string alloc failure */

   /* return filename if it has a valid extension and exists
      (except if it is an .img file (and maybe .gz)) */
   ext = nifti_find_file_extension(fname);
   if ( ext && nifti_fileexists(fname) ) { 
     if ( strncmp(ext,".img",4) != 0 ){
        hdrname = nifti_strdup(fname); 
        return hdrname; 
     }
   }

   /* So the requested name is a basename, contains .img, or does not exist. */
   /* In any case, use basename. */

   /* if .img, look for .hdr, .hdr.gz, .nii, .nii.gz, in that order */
   /* else,    look for .nii, .nii.gz, .hdr, .hdr.gz, in that order */

   /* if we get more extension choices, this could be a loop */

   if ( ext && strncmp(ext,".img",4) != 0 ) efirst = 0;
   else                                     efirst = 1;

   hdrname = (char *)calloc(sizeof(char),strlen(basename)+8);

   strcpy(hdrname,basename);
   strcat(hdrname,elist[efirst]);
   if (nifti_fileexists(hdrname)) { free(basename); return hdrname; }
#ifdef HAVE_ZLIB
   strcat(hdrname,".gz"); 
   if (nifti_fileexists(hdrname)) { free(basename); return hdrname; }
#endif

   /* okay, try the other possibility */

   efirst = 1 - efirst;

   strcpy(hdrname,basename);
   strcat(hdrname,elist[efirst]);
   if (nifti_fileexists(hdrname)) { free(basename); return hdrname; }
#ifdef HAVE_ZLIB
   strcat(hdrname,".gz"); 
   if (nifti_fileexists(hdrname)) { free(basename); return hdrname; }
#endif

   /** if it gets to here then nothing has been found -> failure! **/
   free(basename); 
   free(hdrname); 
   return NULL;
}


/*----------------------------------------------------------------------
 * returns filename of data/img file on success and NULL if no appropriate
 * file could be found
 * NB: it allocates memory for imgname which should be freed
 *   when no longer required
 ----------------------------------------------------------------------*/
char * nifti_findimgname(char* fname , int nifti_type)
{
   char *basename, *imgname, ext[2][5] = { ".nii", ".img" };
   int  first;  /* first extension to use */

   /** check input file(s) for sanity **/

   if( !nifti_validfilename(fname) ) return NULL;

   /** use nifti_type to decide whether to look for .img or .nii **/
   /** if using compression libraries, .gz files are looked for first **/
   basename =  nifti_makebasename(fname);
   imgname = (char *)calloc(sizeof(char),strlen(basename)+8);

   /* test one form, then another (don't assume input type from image type) */
   /* if we get 3 or more extensions, can make a loop here...               */

   if (nifti_type == 1) first = 0;   /* type 1      should match .nii */
   else                 first = 1;   /* type 0 or 2 should match .img */

   strcpy(imgname,basename);
   strcat(imgname,ext[first]);
   if (nifti_fileexists(imgname)) { free(basename); return imgname; }
#ifdef HAVE_ZLIB  /* then also check for .gz */
   strcat(imgname,".gz");
   if (nifti_fileexists(imgname)) { free(basename); return imgname; }
#endif

   /* failed to find image file with expected extension, try the other */

   strcpy(imgname,basename);
   strcat(imgname,ext[1-first]);  /* can do this with only 2 choices */
   if (nifti_fileexists(imgname)) { free(basename); return imgname; }
#ifdef HAVE_ZLIB  /* then also check for .gz */
   strcat(imgname,".gz");
   if (nifti_fileexists(imgname)) { free(basename); return imgname; }
#endif

   /* otherwise - failure */
   free(basename); 
   free(imgname); 
   return NULL;
}
   
/*--------------------------------------------------------------------------*/
/* Determine if this is a NIFTI-formatted file.
   - returns 0 if file looks like ANALYZE 7.5 [checks sizeof_hdr field == 348]
   - returns 1 if file marked as NIFTI (header+data in 1 file)
   - returns 2 if file marked as NIFTI (header+data in 2 files)
   - returns -1 if it can't tell, file doesn't exist, etc.
----------------------------------------------------------------------------*/

int is_nifti_file( char *hname )
{
   struct nifti_1_header nhdr ;
   znzFile fp ;
   int ii ;
   char *tmpname;

   /* bad input name? */

   if( !nifti_validfilename(hname) ) return -1 ;

   /* open file */

   tmpname = nifti_findhdrname(hname);
   if( tmpname == NULL ){
      if( gni_debug > 0 )
         fprintf(stderr,"** no header file found for '%s'\n",hname);
      return -1;
   }
   fp = znzopen( tmpname , "rb" , 1 ) ;
   free(tmpname);
   if (znz_isnull(fp))                      return -1 ;  /* bad open? */

   /* read header, close file */

   ii = znzread( &nhdr , 1 , sizeof(nhdr) , fp ) ;
   znzclose( fp ) ;
   if( ii < sizeof(nhdr) )               return -1 ;  /* bad read? */

   /* check for NIFTI-ness */

   if( NIFTI_VERSION(nhdr) != 0 ){
     return ( NIFTI_ONEFILE(nhdr) ) ? 1 : 2 ;
   }

   /* check for ANALYZE-ness (sizeof_hdr field == 348) */

   ii = nhdr.sizeof_hdr ;
   if( ii == sizeof(nhdr) ) return 0 ;  /* matches */

   /* try byte-swapping header */

   swap_4(ii) ;
   if( ii == sizeof(nhdr) ) return 0 ;  /* matches */

   return -1 ;                          /* not good */
}

static int print_hex_vals( char * data, int nbytes, FILE * fp )
{
   int c;

   if ( !data || nbytes < 1 || !fp ) return -1;

   fputs("0x", fp);
   for ( c = 0; c < nbytes; c++ )
      fprintf(fp, " %x", data[c]);

   return 0;
}

int disp_nifti_1_header( char * info, nifti_1_header * hp )
{
   int c;

   fputs( "-------------------------------------------------------\n", stderr );
   if ( info )  fputs( info, stderr );
   if ( !hp  ){ fputs(" ** no nifti_1_header to display!\n",stderr); return 1; }

   fprintf(stderr," nifti_1_header :\n"
           "    sizeof_hdr     = %d\n"
           "    data_type[10]  = ", hp->sizeof_hdr);
   print_hex_vals(hp->data_type, 10, stderr);
   fprintf(stderr, "\n"
           "    db_name[18]    = ");
   print_hex_vals(hp->db_name, 18, stderr);
   fprintf(stderr, "\n"
           "    extents        = %d\n"
           "    session_error  = %d\n"
           "    regular        = 0x%x\n"
           "    dim_info       = 0x%x\n",
      hp->extents, hp->session_error, hp->regular, hp->dim_info );
   fprintf(stderr, "    dim[8]         =");
   for ( c = 0; c < 8; c++ ) fprintf(stderr," %d", hp->dim[c]);
   fprintf(stderr, "\n"
           "    intent_p1      = %f\n"
           "    intent_p2      = %f\n"
           "    intent_p3      = %f\n"
           "    intent_code    = %d\n"
           "    datatype       = %d\n"
           "    bitpix         = %d\n"
           "    slice_start    = %d\n"
           "    pixdim[8]      =",
           hp->intent_p1, hp->intent_p2, hp->intent_p3, hp->intent_code,
           hp->datatype, hp->bitpix, hp->slice_start);
   /* break pixdim over 2 lines */
   for ( c = 0; c < 4; c++ ) fprintf(stderr," %f", hp->pixdim[c]);
   fprintf(stderr, "\n                    ");
   for ( c = 4; c < 8; c++ ) fprintf(stderr," %f", hp->pixdim[c]);
   fprintf(stderr, "\n"
           "    vox_offset     = %f\n"
           "    scl_slope      = %f\n"
           "    scl_inter      = %f\n"
           "    slice_end      = %d\n"
           "    slice_code     = %d\n"
           "    xyzt_units     = 0x%x\n"
           "    cal_max        = %f\n"
           "    cal_min        = %f\n"
           "    slice_duration = %f\n"
           "    toffset        = %f\n"
           "    glmax          = %d\n"
           "    glmin          = %d\n",
           hp->vox_offset, hp->scl_slope, hp->scl_inter, hp->slice_end,
           hp->slice_code, hp->xyzt_units, hp->cal_max, hp->cal_min,
           hp->slice_duration, hp->toffset, hp->glmax, hp->glmin);
   fprintf(stderr,
           "    descrip        = '%.80s'\n"
           "    aux_file       = '%.24s'\n"
           "    qform_code     = %d\n"
           "    sform_code     = %d\n"
           "    quatern_b      = %f\n"
           "    quatern_c      = %f\n"
           "    quatern_d      = %f\n"
           "    qoffset_x      = %f\n"
           "    qoffset_y      = %f\n"
           "    qoffset_z      = %f\n"
           "    srow_x[4]      = %f, %f, %f, %f\n"
           "    srow_y[4]      = %f, %f, %f, %f\n"
           "    srow_z[4]      = %f, %f, %f, %f\n"
           "    intent_name    = '%-.16s'\n"
           "    magic          = '%-.4s'\n",
           hp->descrip, hp->aux_file, hp->qform_code, hp->sform_code,
           hp->quatern_b, hp->quatern_c, hp->quatern_d,
           hp->qoffset_x, hp->qoffset_y, hp->qoffset_z,
           hp->srow_x[0], hp->srow_x[1], hp->srow_x[2], hp->srow_x[3],
           hp->srow_y[0], hp->srow_y[1], hp->srow_y[2], hp->srow_y[3],
           hp->srow_z[0], hp->srow_z[1], hp->srow_z[2], hp->srow_z[3],
           hp->intent_name, hp->magic);
   fputs( "-------------------------------------------------------\n", stderr );

   return 0;
}


#undef  ERREX
#define ERREX(msg)                                           \
 do{ fprintf(stderr,"** ERROR: nifti_convert_nhdr2nim: %s\n", (msg) ) ;  \
     return NULL ; } while(0)

nifti_image* nifti_convert_nhdr2nim(struct nifti_1_header nhdr, char* fname)
{
  int   ii , doswap , ioff, ndim, nvox ;
  int   is_nifti , is_onefile ;
  short ss ;
  char *iname=NULL;
  nifti_image *nim;

   nim = (nifti_image *) calloc( 1 , sizeof(nifti_image) ) ;
   if( !nim ) ERREX("failed to allocate nifti image");

   /* be explicit with pointers */
   nim->fname = NULL;
   nim->iname = NULL;
   nim->data = NULL;

  /** check if have to swap bytes **/
  
   doswap = 0 ;                                           /* swap data flag */
   ss = nhdr.dim[0] ;
   if( ss != 0 ){                            /* check dim[0] for good value */
     if( ss < 0 || ss > 7 ){
       swap_2(ss) ;
       if( ss < 0 || ss > 7 )            ERREX("bad dim[0]") ;
       doswap = 1 ;
     }
   } else {                       /* dim[0] == 0 is illegal, but does occur */
     ii = nhdr.sizeof_hdr ;            /* so check sizeof_hdr field instead */

     if( ii != sizeof(nhdr) ){
       swap_4(ii) ;
       if( ii != sizeof(nhdr) )          ERREX("bad sizeof_hdr") ;
       doswap = 1 ;
     }
   }

   /** determine if this is a NIFTI-1 compliant header **/

   is_nifti = NIFTI_VERSION(nhdr) ;
   if( doswap ) {
      if ( gni_debug > 2 ) disp_nifti_1_header("-d ni1 pre-swap: ", &nhdr);
      swap_nifti_header( &nhdr , is_nifti ) ;
   }

   if ( gni_debug > 1 ) disp_nifti_1_header("-d nhdr2nim : ", &nhdr);

   if( nhdr.datatype == DT_BINARY ||
       nhdr.datatype == DT_UNKNOWN  )    ERREX("bad datatype") ;

   if( nhdr.dim[1] <= 0 )                ERREX("bad dim[1]") ;

   for( ii=2 ; ii <= 7 ; ii++ )
     if( nhdr.dim[ii] <= 0 ) nhdr.dim[ii] = 1 ;  /* fix bad dim[] values */

   /** get number of dimensions (ignoring dim[0] now) **/

   for( ii=7 ; ii >= 2 ; ii-- )            /* loop backwards until we  */
     if( nhdr.dim[ii] > 1 ) break ;        /* find a dim bigger than 1 */
   ndim = ii ;

   /* set bad grid spacings to 1.0 */

   for( ii=1 ; ii <= 7 ; ii++ ){
     if( nhdr.pixdim[ii] == 0.0         ||
         !IS_GOOD_FLOAT(nhdr.pixdim[ii])  ) nhdr.pixdim[ii] = 1.0 ;
   }

  is_onefile = is_nifti && NIFTI_ONEFILE(nhdr) ;

  if( is_nifti ) nim->nifti_type = (is_onefile) ? 1 : 2 ;
  else           nim->nifti_type = 0 ;
  
  ii = short_order() ;
  if( doswap )   nim->byteorder = REVERSE_ORDER(ii) ;
  else           nim->byteorder = ii ;
  

  /** dimensions of data array **/
  
  nim->ndim = nim->dim[0] = ndim ;
  nim->nx   = nim->dim[1] = nhdr.dim[1]; nvox  = nim->nx;
  nim->ny   = nim->dim[2] = nhdr.dim[2]; nvox *= nim->ny;
  nim->nz   = nim->dim[3] = nhdr.dim[3]; nvox *= nim->nz;
  nim->nt   = nim->dim[4] = nhdr.dim[4]; nvox *= nim->nt;
  nim->nu   = nim->dim[5] = nhdr.dim[5]; nvox *= nim->nu;
  nim->nv   = nim->dim[6] = nhdr.dim[6]; nvox *= nim->nv;
  nim->nw   = nim->dim[7] = nhdr.dim[7]; nvox *= nim->nw; nim->nvox = nvox;
  
  /** type of data in voxels and how many bytes per voxel */
  
  nim->datatype = nhdr.datatype ;
  
  nifti_datatype_sizes( nim->datatype , &(nim->nbyper) , &(nim->swapsize) ) ;
  if( nim->nbyper == 0 ){ free(nim); ERREX("bad datatype"); }
  
  /** grid spacings **/
  
  nim->dx = nim->pixdim[1] = nhdr.pixdim[1] ;
  nim->dy = nim->pixdim[2] = nhdr.pixdim[2] ;
  nim->dz = nim->pixdim[3] = nhdr.pixdim[3] ;
  nim->dt = nim->pixdim[4] = nhdr.pixdim[4] ;
  nim->du = nim->pixdim[5] = nhdr.pixdim[5] ;
  nim->dv = nim->pixdim[6] = nhdr.pixdim[6] ;
  nim->dw = nim->pixdim[7] = nhdr.pixdim[7] ;
  
  /** compute qto_xyz transformation from pixel indexes (i,j,k) to (x,y,z) **/
  
  if( !is_nifti || nhdr.qform_code <= 0 ){ /** default transformation **/
    
    nim->qto_xyz.m[0][0] = nim->dx ;  /* grid spacings */
    nim->qto_xyz.m[1][1] = nim->dy ;  /* along diagonal */
    nim->qto_xyz.m[2][2] = nim->dz ;
    
    /* off diagonal is zero */
    
    nim->qto_xyz.m[0][1]=nim->qto_xyz.m[0][2]=nim->qto_xyz.m[0][3] = 0.0;
    nim->qto_xyz.m[1][0]=nim->qto_xyz.m[1][2]=nim->qto_xyz.m[1][3] = 0.0;
    nim->qto_xyz.m[2][0]=nim->qto_xyz.m[2][1]=nim->qto_xyz.m[2][3] = 0.0;
    
    /* last row is always [ 0 0 0 1 ] */
    
    nim->qto_xyz.m[3][0]=nim->qto_xyz.m[3][1]=nim->qto_xyz.m[3][2] = 0.0;
    nim->qto_xyz.m[3][3]= 1.0 ;
    
    nim->qform_code = NIFTI_XFORM_UNKNOWN ;
    
  } else {                 /** NIFTI: quaternion-specified transformation **/
    
    nim->quatern_b = FIXED_FLOAT( nhdr.quatern_b ) ;
    nim->quatern_c = FIXED_FLOAT( nhdr.quatern_c ) ;
    nim->quatern_d = FIXED_FLOAT( nhdr.quatern_d ) ;
    
    nim->qoffset_x = FIXED_FLOAT(nhdr.qoffset_x) ;
    nim->qoffset_y = FIXED_FLOAT(nhdr.qoffset_y) ;
    nim->qoffset_z = FIXED_FLOAT(nhdr.qoffset_z) ;
    
    nim->qfac = (nhdr.pixdim[0] < 0.0) ? -1.0 : 1.0 ;  /* left-handedness? */
    
    nim->qto_xyz = quatern_to_mat44(
                      nim->quatern_b, nim->quatern_c, nim->quatern_d,
                      nim->qoffset_x, nim->qoffset_y, nim->qoffset_z,
                      nim->dx       , nim->dy       , nim->dz       ,
                      nim->qfac                                      ) ;

    nim->qform_code = nhdr.qform_code ;

    if( gni_debug > 1 )
       nifti_disp_matrix_orient("-d qform orientations:\n", nim->qto_xyz);
  }
  
  /** load inverse transformation (x,y,z) -> (i,j,k) **/
  
  nim->qto_ijk = mat44_inverse( nim->qto_xyz ) ;
  
  /** load sto_xyz affine transformation, if present **/
  
  if( !is_nifti || nhdr.sform_code <= 0 ){ /** no sto transformation **/
    
    nim->sform_code = NIFTI_XFORM_UNKNOWN ;
    
  } else {                            /** sto transformation from srow_*[] **/
    
    nim->sto_xyz.m[0][0] = nhdr.srow_x[0] ;
    nim->sto_xyz.m[0][1] = nhdr.srow_x[1] ;
    nim->sto_xyz.m[0][2] = nhdr.srow_x[2] ;
    nim->sto_xyz.m[0][3] = nhdr.srow_x[3] ;
    
    nim->sto_xyz.m[1][0] = nhdr.srow_y[0] ;
    nim->sto_xyz.m[1][1] = nhdr.srow_y[1] ;
    nim->sto_xyz.m[1][2] = nhdr.srow_y[2] ;
    nim->sto_xyz.m[1][3] = nhdr.srow_y[3] ;
    
    nim->sto_xyz.m[2][0] = nhdr.srow_z[0] ;
    nim->sto_xyz.m[2][1] = nhdr.srow_z[1] ;
    nim->sto_xyz.m[2][2] = nhdr.srow_z[2] ;
    nim->sto_xyz.m[2][3] = nhdr.srow_z[3] ;
    
    /* last row is always [ 0 0 0 1 ] */
    
    nim->sto_xyz.m[3][0]=nim->sto_xyz.m[3][1]=nim->sto_xyz.m[3][2] = 0.0;
    nim->sto_xyz.m[3][3]= 1.0 ;
    
    nim->sto_ijk = mat44_inverse( nim->sto_xyz ) ;
    
    nim->sform_code = nhdr.sform_code ;

    if( gni_debug > 1 )
       nifti_disp_matrix_orient("-d qform orientations:\n", nim->sto_xyz);
  }
  
  /* miscellaneous NIFTI stuff */
  
  if( is_nifti ){
    nim->scl_slope   = FIXED_FLOAT( nhdr.scl_slope ) ;
    nim->scl_inter   = FIXED_FLOAT( nhdr.scl_inter ) ;
    
    nim->intent_code = nhdr.intent_code ;
    
    nim->intent_p1 = FIXED_FLOAT( nhdr.intent_p1 ) ;
    nim->intent_p2 = FIXED_FLOAT( nhdr.intent_p2 ) ;
    nim->intent_p3 = FIXED_FLOAT( nhdr.intent_p3 ) ;
    
    nim->toffset   = FIXED_FLOAT( nhdr.toffset ) ;
    
    memcpy(nim->intent_name,nhdr.intent_name,15); nim->intent_name[15] = '\0';
    
    nim->xyz_units  = XYZT_TO_SPACE(nhdr.xyzt_units) ;
    nim->time_units = XYZT_TO_TIME (nhdr.xyzt_units) ;
    
    nim->freq_dim  = DIM_INFO_TO_FREQ_DIM ( nhdr.dim_info ) ;
    nim->phase_dim = DIM_INFO_TO_PHASE_DIM( nhdr.dim_info ) ;
    nim->slice_dim = DIM_INFO_TO_SLICE_DIM( nhdr.dim_info ) ;
    
    nim->slice_code     = nhdr.slice_code  ;
    nim->slice_start    = nhdr.slice_start ;
    nim->slice_end      = nhdr.slice_end   ;
    nim->slice_duration = FIXED_FLOAT(nhdr.slice_duration) ;
  }
  
  /* Miscellaneous ANALYZE stuff */
  
  nim->cal_min = FIXED_FLOAT(nhdr.cal_min) ;
  nim->cal_max = FIXED_FLOAT(nhdr.cal_max) ;
  
  memcpy(nim->descrip ,nhdr.descrip ,79) ; nim->descrip [79] = '\0' ;
  memcpy(nim->aux_file,nhdr.aux_file,23) ; nim->aux_file[23] = '\0' ;
  
   /** will read image data from file 'iname' starting at offset 'ioff' **/

   is_onefile = is_nifti && NIFTI_ONEFILE(nhdr) ;

   if( is_onefile ){
     ioff = (int)nhdr.vox_offset ;
     if( ioff < sizeof(nhdr) ) ioff = sizeof(nhdr) ;
   } else {
     ioff = (int)nhdr.vox_offset ;
   }
   nim->iname_offset = ioff ;


   /* deal with file names if set */
   if (fname!=NULL) {
     nim->fname = nifti_strdup(fname);
     /* determine name of image, if not already set */
     if (nim->iname==NULL) {
       if (is_onefile) {
	 iname = nifti_strdup(nim->fname);
       } else {
	 iname = nifti_findimgname(nim->fname,nim->nifti_type);
	 if (iname==NULL)  ERREX("bad filename");
       }
       /* don't free iname, as now nim->iname is using this storage */
       nim->iname        = iname ;          /* save image filename */
     }
   } else { 
     nim->fname = NULL;  
     nim->iname = NULL; 
   }

   /* clear extension fields */
   nim->num_ext = 0;
   nim->ext_list = NULL;

   return nim;
}


/*--------------------------------------------------------------------------*/
/* Read in a NIFTI-1 or ANALYZE-7.5 file (pair) into a nifti_image struct.
    - Input is .hdr or .nii filename.
    - Return value is NULL if something fails badly.
    - If read_data parameter is nonzero, the image data will actually
      be read in; otherwise, it will have to be read later
      (e.g., using the nifti_image_load() function).
    - The image data will be stored in whatever data format the
      input data is; no scaling will be applied.
    - DT_BINARY data is not supported!
    - nifti_image_free() can be used to delete the returned struct,
      when you are done with it.
----------------------------------------------------------------------------*/

#undef  ERREX
#define ERREX(msg)                                           \
 do{ fprintf(stderr,"** ERROR: nifti_image_open(%s): %s\n",  \
             (hname != NULL) ? hname : "(null)" , (msg) ) ;  \
     return fptr ; } while(0)

static znzFile nifti_image_open( char *hname , char *opts , nifti_image **nim)
{
  znzFile fptr=NULL;
  znz_setnull(fptr);

  /* open the hdr and reading it in, but do not load the data  */
  *nim = nifti_image_read(hname,0);

  /* open the image file, ready for reading
   * - NB: compressed works for all reads */
  if( ((*nim) == NULL)      || ((*nim)->iname == NULL) ||
      ((*nim)->nbyper <= 0) || ((*nim)->nvox <= 0)       ) ERREX("bad header info") ;

  /** open image data file **/  
  fptr = znzopen((*nim)->iname ,opts,1);
  if( znz_isnull(fptr) ) ERREX("Can't open data file") ;
  
  return fptr;
}


/* use macro LNI_FILE_ERROR instead of ERREX()
#undef  ERREX
#define ERREX(msg)                                           \
 do{ fprintf(stderr,"** ERROR: nifti_image_read(%s): %s\n",  \
             (hname != NULL) ? hname : "(null)" , (msg) ) ;  \
     return NULL ; } while(0)
*/

nifti_image *nifti_image_read( char *hname , int read_data )
{
   struct nifti_1_header  nhdr ;
   nifti_image           *nim ;
   znzFile                fp ;
   int                    ii , ilen, filesize, remaining;
   char                   fname[] = { "nifti_image_read" };
   char                   buf[16] , *workingname=NULL;
   
   /* determine file name to use for header */
   workingname = nifti_findhdrname(hname);
   if( workingname == NULL ){
      if( gni_debug > 0 )
         LNI_FERR(fname,"failed to find header file for", hname);
      return NULL;  /* check return */
   } else if( gni_debug > 1 )
      fprintf(stderr,"-d %s: found header filename '%s'\n",fname,workingname);

   if( nifti_is_gzfile(workingname) )
      filesize = -1;    /* cannot determine size of compressed file */
   else
      filesize = get_filesize(workingname);

   fp = znzopen(workingname,"rb",1);
   if( znz_isnull(fp) ){
      if( gni_debug > 0 )
         LNI_FERR(fname,"failed to open header file",workingname);
      free(workingname);
      return NULL;
   }

   /** test if header file starts with ASCII string "<nifti_image";
       if so, read the dataset that special way and return now
       (this is NOT part of the NIFTI-1 standard!)                **/

   ii = znzread( buf , 1 , 12 , fp ) ;
   if( ii < 12 ){
      znzclose( fp ) ;
      if( gni_debug > 0 )
         LNI_FERR(fname,"short header read",workingname);
         free(workingname);
         return NULL;
   }

   znzrewind(fp) ;
   buf[12] = '\0' ;
   if( strcmp(buf,"<nifti_image") == 0 ){  /* have an ASCII header! */
     int slen, txt_size;
     char  *sbuf ;
     if( nifti_is_gzfile(workingname) ){
        LNI_FERR(fname, "compressed file with negative offset", workingname);
        free(workingname);
        return NULL;
     }
     slen = filesize;

     if( gni_debug > 1 )
        fprintf(stderr,"-d %s: have ASCII NIFTI file of size %d\n",fname,slen);

     if( slen > 65530 ) slen = 65530 ;
     sbuf = (char *)calloc(sizeof(char),slen+1) ;
     znzread( sbuf , 1 , slen , fp ) ;
     nim = nifti_image_from_ascii( sbuf, &txt_size ) ; free( sbuf ) ;
     if( nim == NULL ){
        LNI_FERR(fname,"failed nifti_image_from_ascii()",workingname);
        free(workingname);
        return NULL;
     }
     nim->nifti_type = 3 ;

     /* compute remaining space for extensions */
     remaining = filesize - txt_size - (int)nifti_get_volsize(nim);
     if( remaining > 4 ){
        /* read extensions (reposition file pointer, first) */
        znzseek(fp, txt_size, SEEK_SET);
        (void) nifti_read_extensions(nim, fp, remaining);
     }

     znzclose( fp ) ;

     nim->iname_offset = -1 ;  /* now check from the end of the file */

     if( read_data ) ii = nifti_image_load( nim ) ;
     else            nim->data = NULL ;

     free(workingname);

     /* check for nifti_image_load() failure, maybe bail out */
     if( read_data && ii != 0 ){
        if( gni_debug > 1 )
           fprintf(stderr,"-d failed image_load, free nifti image struct\n");
        free(nim);
        return NULL;
     }

     return nim ;
   }

   /**================ Normal case:  read binary header ===================*/

   ii = znzread( &nhdr , 1 , sizeof(nhdr) , fp ) ;       /* read the thing */

   /* keep the file open for a while so we can check for extensions
    * after nifti_convert_nhdr2nim() */

   if( ii < sizeof(nhdr) ){
      if( gni_debug > 0 ){
         LNI_FERR(fname,"bad binary header read for file", workingname);
         fprintf(stderr,"  - read %d of %d bytes\n",ii, (int)sizeof(nhdr));
      }
      free(workingname);
      return NULL;
   }

   /*=== create output image struct and set it up ===*/

   /** convert all nhdr fields to nifti_image fields **/
   nim = nifti_convert_nhdr2nim(nhdr,workingname);

   if( nim == NULL ){
      znzclose( fp ) ;                                   /* close the file */
      if( gni_debug > 0 )
         LNI_FERR(fname,"failed to create nifti image from header",workingname);
      free(workingname); /* had to save this for debug message */
      return NULL;
   }

   if( gni_debug > 1 ){
      fprintf(stderr,"+d nifti_image_read(), have nifti image:\n");
      nifti_image_infodump(nim);
   }

   /* check for extensions, let us just ignore any errors here */
   if( NIFTI_ONEFILE(nhdr) ) remaining = nim->iname_offset - sizeof(nhdr);
   else                      remaining = filesize - sizeof(nhdr);

   if( remaining > 4 ) (void) nifti_read_extensions(nim, fp, remaining);

   znzclose( fp ) ;                                      /* close the file */
   free(workingname);
   /* check that the data file is OK */
   if (!nifti_is_gzfile(nim->iname)) {
     /* can't use get_filesize on a compressed file   :( */
     ilen = get_filesize(nim->iname) ;
     if( ilen <= nim->iname_offset ){
        fprintf(stderr,"ERROR %s: file size less than offset (%d < %d)\n",
                fname,ilen,nim->iname_offset);
        free(nim);
        return NULL;
     }
   }

   /** read the data if desired, then bug out **/
   if( read_data ) ii = nifti_image_load( nim ) ;
   else            nim->data = NULL ;

   return nim ;
}


/*----------------------------------------------------------------------
 * Read the extensions into the nifti_image struct   08 Dec 2004 [rickr]
 *
 * This function is called just after the header struct is read in, and
 * it is assumed the file pointer has not moved.  The value in remain
 * is assumed to be accurate, reflecting the bytes of space for potential
 * extensions.
 *
 * return the number of extensions read in, or < 0 on error
 *----------------------------------------------------------------------*/
static int nifti_read_extensions( nifti_image *nim, znzFile fp, int remain )
{
   nifti1_extender    extdr;      /* defines extension existance  */
   nifti1_extension   extn;       /* single extension to process  */
   nifti1_extension * Elist;      /* list of processed extensions */
   int                posn, count;

   if( !nim || znz_isnull(fp) ) {
      if( gni_debug > 0 )
         fprintf(stderr,"** nifti_read_extensions: bad inputs (%p,%p)\n",
                 nim, fp);
      return -1;
   }

   posn = znztell(fp);
 
   if( (posn != sizeof(nifti_1_header)) && (nim->nifti_type != 3) )
      fprintf(stderr,"** WARNING: posn not header size (%d, %d)\n",
              posn, (int)sizeof(nifti_1_header));

   if( gni_debug > 2 )
      fprintf(stderr,"-d nre: posn = %d, offset = %d, type = %d, remain = %d\n",
              posn, nim->iname_offset, nim->nifti_type, remain);

   if( remain < 16 ){
      if( gni_debug > 2 ) fprintf(stderr,"no space for extensions\n");
      return 0;
   }

   count = znzread( extdr.extension, 1, 4, fp ); /* get extender */

   if( count < 4 ){
      if( gni_debug > 1 )
         fprintf(stderr,"-d no extender stored in header file '%s'\n",
                 nim->fname);
      return 0;
   }

   if( extdr.extension[0] != 1 ){
      if( gni_debug > 1 )
         fprintf(stderr,"-d extender[0] (%d) shows no extensions for '%s'\n",
                 extdr.extension[0], nim->fname);
      return 0;
   }

   /* so we expect extensions, but have no idea of how many there may be */

   count = 0;
   Elist = NULL;
   while (nifti_read_next_extension(&extn, nim, remain, fp) > 0)
   {
      if( nifti_add_exten_to_list(&extn, &Elist, count+1) < 0 ){
         if( gni_debug > 0 )
            fprintf(stderr,"** failed adding ext %d to list\n", count);
         return -1;
      }

      /* we have a new extension */
      if( gni_debug > 1 ){
         fprintf(stderr,"+d found extension #%d, code = 0x%x, size = %d\n",
                 count, extn.ecode, extn.esize);
         if( extn.ecode == NIFTI_ECODE_AFNI )  /* if AFNI, then ~XML */
            fprintf(stderr,"   AFNI extension: %.*s\n",extn.esize-8,extn.edata);
      }
      if( remain > 0 ) remain -= extn.esize;
      count++;
   }

   if( gni_debug > 2 ) fprintf(stderr,"+d found %d extension(s)\n", count);

   nim->num_ext = count;
   nim->ext_list = Elist;

   return count;
}


/*----------------------------------------------------------------------
 * nifti_add_exten_to_list     - add new extension to the list
 *
 * We will append via malloc, copy and free, because on an error,
 * the old data pointers must all be released.
 *
 * return:
 *     0 on success
 *    -1 on error     (and free the entire list)
 *----------------------------------------------------------------------*/
static int nifti_add_exten_to_list( nifti1_extension *  new_ext,
                                    nifti1_extension ** list, int new_length )
                                      
{
   nifti1_extension * tmplist;
   int                count;
  
   tmplist = *list;
   *list = (nifti1_extension *)malloc(new_length * sizeof(nifti1_extension));

   /* check for failure first */
   if( ! *list ){
      fprintf(stderr,"** failed to alloc %d extension structs (%d bytes)\n",
              new_length, new_length*(int)sizeof(nifti1_extension));
      if( !tmplist ) return -1;  /* no old list to lose */

      for ( count = 0; count < new_length-1; count++ )
         if( tmplist[count].edata ) free(tmplist[count].edata);
      free(tmplist);
      return -1;
   }

   /* we have memory, so copy the old and insert the new */
   memcpy(*list, tmplist, (new_length-1)*sizeof(nifti1_extension));

   /* for some reason, I just don't like struct copy... */
   (*list)[new_length-1].esize = new_ext->esize;
   (*list)[new_length-1].ecode = new_ext->ecode;
   (*list)[new_length-1].edata = new_ext->edata;

   return 0;
}


/*----------------------------------------------------------------------
 * nifti_read_next_extension  - read a single extension from the file
 *
 * return (>= 0 is okay):
 *
 *     success      : esize
 *     no extension : 0
 *     error        : -1
 *----------------------------------------------------------------------*/
static int nifti_read_next_extension( nifti1_extension * nex, nifti_image *nim,
                                      int remain, znzFile fp )
{
   int swap = nim->byteorder != short_order();
   int count, size, code;

   /* first clear nex */
   nex->esize = nex->ecode = 0;
   nex->edata = NULL;

   if( remain < 16 ){
      if( gni_debug > 2 )
         fprintf(stderr,"-d only %d bytes remain, so no extension\n", remain);
      return 0;
   }

   /* must start with 4-byte size and code */
   count = znzread( &size, 4, 1, fp );
   if( count == 1 ) count += znzread( &code, 4, 1, fp );

   if( count != 2 ){
      if( gni_debug > 2 ) fprintf(stderr,"-d current extension read failed\n");
      znzseek(fp, -4*count, SEEK_CUR); /* back up past any read */
      return 0;                        /* no extension, no error condition */
   }

   if( swap ){
      if( gni_debug > 2 )
         fprintf(stderr,"-d pre-swap exts: code %d, size %d\n", code, size);

      swap_4bytes(1, &size);
      swap_4bytes(1, &code);
   }

   if( gni_debug > 2 )
      fprintf(stderr,"-d potential extension: code %d, size %d\n", code, size);

   if( !nifti_valid_extension(nim, size, code, remain) ){
      if( znzseek(fp, -8, SEEK_CUR) < 0 ){      /* back up past any read */
         fprintf(stderr,"** failure to back out of extension read!\n");
         return -1;
      }
      return 0;
   }

   /* now get the actual data */
   nex->esize = size;
   nex->ecode = code;

   size -= 8;  /* subtract space for size and code in extension */
   nex->edata = (char *)malloc(size * sizeof(char));
   if( !nex->edata ){
      fprintf(stderr,"** failed to allocate %d bytes for extension\n",size);
      return -1;
   }

   count = znzread(nex->edata, 1, size, fp);
   if( count < size ){
      if( gni_debug > 0 )
         fprintf(stderr,"-d read only %d (of %d) bytes for extension\n",
                 count, size);
      free(nex->edata);
      nex->edata = NULL;
      return -1;
   }

   /* success! */
   if( gni_debug > 1 )
      fprintf(stderr,"+d successfully read extension, code %d, size %d\n",
              nex->ecode, nex->esize);

   return nex->esize;
}

/*----------------------------------------------------------------------
 * check for valid size and code, as well as can be done
 *----------------------------------------------------------------------*/
static int nifti_valid_extension(nifti_image *nim, int size, int code, int rem)
{
   /* check for bad code before bad size */
   if( code != 0 &&          /* unregistered, not recommended, but valid */
       code != 2 &&          /* DICOM */
       code != 4   )         /* AFNI */
   {
      if( gni_debug > 2 ) fprintf(stderr,"-d invalid extension code %d\n",code);
      return 0;
   }

   if( size < 16 ){
      if( gni_debug > 2 ) fprintf(stderr,"-d ext size %d, no extension\n",size);
      return 0;
   }

   if( size > rem ){
      if( gni_debug > 2 )
         fprintf(stderr,"-d ext size %d, space %d, no extension\n", size, rem);
      return 0;
   }

   if( size & 0xf ){
      if( gni_debug > 2 )
         fprintf(stderr,"-d nifti extension size %d not multiple of 16\n",size);
      return 0;
   }

   if( nim->nifti_type == 3 && size > LNI_MAX_NIA_EXT_LEN ){
      if( gni_debug > 2 )
         fprintf(stderr,"-d NVE, bad nifti_type 3 size %d\n", size);
      return 0;
   }

   return 1;
}


/*----------------------------------------------------------------------
 * nifti_image_load_prep  - prepare to read data
 *
 * Check nifti_image fields, open the file and seek to the appropriate
 * offset for reading.
 *
 * return NULL on failure
 *----------------------------------------------------------------------*/
static znzFile nifti_image_load_prep( nifti_image *nim )
{
   /* set up data space, open data file and seek, then call nifti_read_buffer */
   size_t ntot , ii , ioff;
   znzFile fp;
   char    fname[] = { "nifti_image_load_prep" };

   if( nim == NULL      || nim->iname == NULL ||
       nim->nbyper <= 0 || nim->nvox <= 0       )
   {
      if ( gni_debug > 0 ){
         if( !nim ) fprintf(stderr,"** ERROR: N_image_load: no nifti image\n");
         else fprintf(stderr,"** ERROR: N_image_load: bad params (%p,%d,%d)\n",
                      nim->iname, nim->nbyper, nim->nvox);
      }
      return NULL;
   }

   ntot = nifti_get_volsize(nim) ; /* total bytes to read */

   /** open image data file **/

   fp = znzopen(nim->iname , "rb",1);
   if( znz_isnull(fp) ){
      if( gni_debug > 0 )
         LNI_FERR(fname,"cannot open data file",nim->iname);
      return NULL;
   }

   /* negative offset means to figure from end of file */
   if( nim->iname_offset < 0 ){
     if( nifti_is_gzfile(nim->iname) ){
        if( gni_debug > 0 )
           LNI_FERR(fname,"negative offset for compressed file",nim->iname);
        znzclose(fp);
        return NULL;
     }
     ii = get_filesize( nim->iname ) ;
     if( ii <= 0 ){
        if( gni_debug > 0 )
           LNI_FERR(fname,"empty data file",nim->iname);
        znzclose(fp);
        return NULL;
     }
     ioff = (ii > ntot) ? ii-ntot : 0 ;
   } else {                              /* non-negative offset   */
     ioff = nim->iname_offset ;          /* means use it directly */
   }
   znzseek( fp , ioff , SEEK_SET ) ;

   return fp;
}


/*----------------------------------------------------------------------
 * Load the image data from disk into an already-prepared image struct.
 *
 * change to return an int:                    29 Nov 2004 [rickr]
 *    0 : success (image data space will be allocated if it wasn't already)
 *  < 0 : error
 *----------------------------------------------------------------------*/
int nifti_image_load( nifti_image *nim )
{
   /* set up data space, open data file and seek, then call nifti_read_buffer */
   size_t ntot , ii ;
   znzFile fp ;

   fp = nifti_image_load_prep( nim );

   if( fp == NULL ){
      if( gni_debug > 0 )
         fprintf(stderr,"** nifti_image_load, failed load_prep\n");
      return -1;
   }

   ntot = nifti_get_volsize(nim);

   /* if the data pointer is already set, use the given space */

   if( nim->data == NULL )
   {
     nim->data = (void *)calloc(1,ntot) ;  /* create image memory */
     if( nim->data == NULL ){
        if( gni_debug > 0 )
           fprintf(stderr,"** failed to alloc %d bytes for image data\n",
                   (int)ntot);
        znzclose(fp);
        return -1;
     }
   }

   /* now that everything is set up - do the reading */
   ii = nifti_read_buffer(fp,nim->data,ntot,nim);
   if( ii < ntot ){
      znzclose(fp) ;
      free(nim->data) ;
      nim->data = NULL ;
      return -1 ;  /* errors were printed in nifti_read_buffer() */
   }

   znzclose( fp ) ;

   return 0 ;
}


/* 30 Nov 2004 [rickr]
#undef  ERREX
#define ERREX(msg)                                               \
 do{ fprintf(stderr,"** ERROR: nifti_read_buffer: %s\n",(msg)) ;  \
     return 0; } while(0)
*/


/*----------------------------------------------------------------------
 * reads ntot bytes of data from an open file and byte swaps if necessary
 * note that nifti_image is required for information on datatype, etc.
 *----------------------------------------------------------------------*/
static size_t nifti_read_buffer(znzFile fp, void* dataptr, size_t ntot, 
                                nifti_image *nim)
{
  size_t ii;

  if( dataptr == NULL ){
     if( gni_debug > 0 )
        fprintf(stderr,"** ERROR: nifti_read_buffer: NULL dataptr\n");
     return -1;
  }

  ii = znzread( dataptr , 1 , ntot , fp ) ;             /* data input */
  
  /* if read was short, fail */
  if( ii < ntot ){ 
    if( gni_debug > 0 )
       fprintf(stderr,"++ WARNING: nifti_read_buffer(%s):\n"
	       "   data bytes needed = %u\n"
	       "   data bytes input  = %u\n"
	       "   number missing    = %u (set to 0)\n",
	       nim->iname , (unsigned int)ntot ,
	       (unsigned int)ii , (unsigned int)(ntot-ii) ) ;
    /* memset( (char *)(dataptr)+ii , 0 , ntot-ii ) ;  now failure [rickr] */
    return -1 ;
  }
  
  /* byte swap array if needed */
  
  if( nim->swapsize > 1 && nim->byteorder != short_order() )
    swap_Nbytes( ntot / nim->swapsize , nim->swapsize , dataptr ) ;
  
#ifdef isfinite
{
  /* check input float arrays for goodness, and fix bad floats */
  int fix_count = 0 ;
  
  switch( nim->datatype ){
    
    case NIFTI_TYPE_FLOAT32:
    case NIFTI_TYPE_COMPLEX64:{
        register float *far = (float *)dataptr ; register int jj,nj ;
        nj = ntot / sizeof(float) ;
        for( jj=0 ; jj < nj ; jj++ )   /* count fixes 30 Nov 2004 [rickr] */
           if( !IS_GOOD_FLOAT(far[jj]) ){
              far[jj] = 0 ;
              fix_count++ ;
           }
      }
      break ;
    
    case NIFTI_TYPE_FLOAT64:
    case NIFTI_TYPE_COMPLEX128:{
        register double *far = (double *)dataptr ; register int jj,nj ;
        nj = ntot / sizeof(double) ;
        for( jj=0 ; jj < nj ; jj++ )   /* count fixes 30 Nov 2004 [rickr] */
           if( !IS_GOOD_FLOAT(far[jj]) ){
              far[jj] = 0 ;
              fix_count++ ;
           }
      }
      break ;
    
  }

  if( gni_debug > 1 )
     fprintf(stderr,"+d in image, %d bad floats were set to 0\n", fix_count);
}
#endif
  
  return ii;
}

/*--------------------------------------------------------------------------*/
/* Unload the data in a nifti_image struct, but keep the metadata.
----------------------------------------------------------------------------*/

void nifti_image_unload( nifti_image *nim )
{
   if( nim != NULL && nim->data != NULL ){
     free(nim->data) ; nim->data = NULL ;
   }
   return ;
}

/*--------------------------------------------------------------------------*/
/* Free a nifti_image struct that was read by nifti_image_read().
----------------------------------------------------------------------------*/

void nifti_image_free( nifti_image *nim )
{
   if( nim == NULL ) return ;
   if( nim->fname != NULL ) free(nim->fname) ;
   if( nim->iname != NULL ) free(nim->iname) ;
   if( nim->data  != NULL ) free(nim->data ) ;
   free(nim) ; return ;
}

/*--------------------------------------------------------------------------*/
/* Print to stdout some info about a nifti_image struct.
----------------------------------------------------------------------------*/

void nifti_image_infodump( nifti_image *nim )
{
   char *str = nifti_image_to_ascii( nim ) ;
   /* stdout -> stderr   2 Dec 2004 [rickr] */
   if( str != NULL ){ fputs(str,stderr) ; free(str) ; }
   return ;
}

/*--------------------------------------------------------------------------*/
/* Write a nifti_image to disk.  The following fields of nim affect how
   the output appears:
    - nifti_type = 0 ==> ANALYZE-7.5 format file pair will be written
    - nifti_type = 1 ==> NIFTI-1 format single file will be written
                         (data offset will be 348)
    - nifti_type = 2 ==> NIFTI_1 format file pair will be written
    - nifti_type = 3 ==> NIFTI_1 ASCII single file will be written
    - fname is the name of the output file (header or header+data)
    - if a file pair is being written, iname is the name of the data file
    - existing files WILL be overwritten with extreme prejudice
    - if qform_code > 0, the quatern_*, qoffset_*, and qfac fields determine
      the qform output, NOT the qto_xyz matrix; if you want to compute these
      fields from the qto_xyz matrix, you can use the utility function
      mat44_to_quatern()
----------------------------------------------------------------------------*/

static size_t nifti_write_buffer(znzFile fp, void *buffer, size_t numbytes)
{
   /** Write all the image data at once (no swapping here) **/
   size_t ss;
   if (znz_isnull(fp)){
      fprintf(stderr,"** ERROR: nifti_write_buffer: null file pointer\n");
      return 0;
   }
   ss = znzwrite( (void *) buffer , 1 , numbytes , fp ) ;
   return ss;
}


/* just print...
#undef  ERREX
#define ERREX(msg)                                                \
 do{ fprintf(stderr,"** ERROR: nifti_image_write: %s\n",(msg)) ;  \
     return ; } while(0)
*/

static void nifti_write_all_data(znzFile fp, nifti_image *nim,
                                 nifti_brick_list * NBL)
{
   /** Write all the image data at once (no swapping here) **/
   size_t ss;
   int    bnum;

   if( !NBL ){ /* just write one buffer and get out of here */
      ss = nifti_write_buffer(fp,nim->data,nim->nbyper * nim->nvox);
      if (ss < (nim->nbyper * nim->nvox)){
         fprintf(stderr,
            "** ERROR: nifti_image_write: wrote only %d of %d bytes to file\n",
            (int)ss, nim->nbyper * nim->nvox);
         return;
      }

      if( gni_debug > 1 )
         fprintf(stderr,"+d wrote single image of %d bytes\n",(int)ss);
   } else {
      if( ! NBL->bricks || NBL->nbricks <= 0 || NBL->bsize <= 0 ){
         fprintf(stderr,"** write NBL: no brick data to write (%p,%d,%d)\n",
                 NBL->bricks, NBL->nbricks, NBL->bsize);
         return;
      }

      for( bnum = 0; bnum < NBL->nbricks; bnum++ ){
         ss = nifti_write_buffer(fp, NBL->bricks[bnum], NBL->bsize);
         if( ss < NBL->bsize ){
            fprintf(stderr,
               "** write ERROR: wrote %d of %d bytes of brick %d of %d to file",
               (int)ss, NBL->bsize, bnum+1, NBL->nbricks);
            return;
         }
      }
      if( gni_debug > 1 )
         fprintf(stderr,"+d wrote image of %d brick(s), each of %d bytes\n",
                 NBL->nbricks, NBL->bsize);
   }

   nim->byteorder = short_order() ;  /* mark as being in this CPU byte order */

   return ;
}

/* return number of extensions written, or -1 on error */
static int nifti_write_extensions(znzFile fp, nifti_image *nim)
{
   nifti1_extension * list;
   char               extdr[4] = { 0, 0, 0, 0 };
   int                c, size, ok = 1;

   if( znz_isnull(fp) || !nim || nim->num_ext < 0 ){
      if( gni_debug > 0 )
         fprintf(stderr,"** nifti_write_extensions, bad params\n");
      return -1;
   }

   /* write out extender block */
   if( nim->num_ext > 0 ) extdr[0] = 1;
   if( nifti_write_buffer(fp, extdr, 4) != 4 ){
      fprintf(stderr,"** failed to write extender\n");
      return -1;
   }

   list = nim->ext_list;
   for ( c = 0; c < nim->num_ext; c++ ){
      size = nifti_write_buffer(fp, &list->esize, sizeof(int));
      ok = (size == (int)sizeof(int));
      if( ok )
         size = nifti_write_buffer(fp, &list->ecode, sizeof(int));
      ok = (size == (int)sizeof(int));
      if( ok )
         size = nifti_write_buffer(fp, list->edata, list->esize);

      if( !ok ){
         fprintf(stderr,"** failed while writing extension #%d\n",c);
         return -1;
      }

      list++;
   }

   if( gni_debug > 1 )
      fprintf(stderr,"+d wrote out %d extension(s)\n", nim->num_ext);

   return nim->num_ext;
}


nifti_image* nifti_simple_init_nim()
{
  nifti_image *nim;
  struct nifti_1_header nhdr;
  int nbyper, swapsize;
  
  memset(&nhdr,0,sizeof(nhdr)) ;  /* zero out header, to be safe */
  
   /** load the ANALYZE-7.5 generic parts of the header struct **/

   nhdr.sizeof_hdr = sizeof(nhdr) ;
   nhdr.regular    = 'r' ;             /* for some stupid reason */

   nhdr.dim[0] = 3 ;
   nhdr.dim[1] = 1 ; nhdr.dim[2] = 1 ; nhdr.dim[3] = 1 ;
   nhdr.dim[4] = 0 ;

   nhdr.pixdim[0] = 0.0 ;
   nhdr.pixdim[1] = 1.0 ; nhdr.pixdim[2] = 1.0 ;
   nhdr.pixdim[3] = 1.0 ;

   nhdr.datatype = NIFTI_TYPE_FLOAT32 ;
   nifti_datatype_sizes( nhdr.datatype , &nbyper, &swapsize );
   nhdr.bitpix   = 8 * nbyper ;

   nim = nifti_convert_nhdr2nim(nhdr,NULL);
   nim->fname = NULL;
   nim->iname = NULL;
   return nim;
}




struct nifti_1_header nifti_convert_nim2nhdr(nifti_image* nim)
{
  struct nifti_1_header nhdr;

   memset(&nhdr,0,sizeof(nhdr)) ;  /* zero out header, to be safe */


   /** load the ANALYZE-7.5 generic parts of the header struct **/

   nhdr.sizeof_hdr = sizeof(nhdr) ;
   nhdr.regular    = 'r' ;             /* for some stupid reason */

   nhdr.dim[0] = nim->ndim ;
   nhdr.dim[1] = nim->nx ; nhdr.dim[2] = nim->ny ; nhdr.dim[3] = nim->nz ;
   nhdr.dim[4] = nim->nt ; nhdr.dim[5] = nim->nu ; nhdr.dim[6] = nim->nv ;
   nhdr.dim[7] = nim->nw ;

   nhdr.pixdim[0] = 0.0 ;
   nhdr.pixdim[1] = nim->dx ; nhdr.pixdim[2] = nim->dy ;
   nhdr.pixdim[3] = nim->dz ; nhdr.pixdim[4] = nim->dt ;
   nhdr.pixdim[5] = nim->du ; nhdr.pixdim[6] = nim->dv ;
   nhdr.pixdim[7] = nim->dw ;

   nhdr.datatype = nim->datatype ;
   nhdr.bitpix   = 8 * nim->nbyper ;

   if( nim->cal_max > nim->cal_min ){
     nhdr.cal_max = nim->cal_max ;
     nhdr.cal_min = nim->cal_min ;
   }

   if( nim->scl_slope != 0.0 ){
     nhdr.scl_slope = nim->scl_slope ;
     nhdr.scl_inter = nim->scl_inter ;
   }

   if( nim->descrip[0] != '\0' ){
     memcpy(nhdr.descrip ,nim->descrip ,79) ; nhdr.descrip[79] = '\0' ;
   }
   if( nim->aux_file[0] != '\0' ){
     memcpy(nhdr.aux_file ,nim->aux_file ,23) ; nhdr.aux_file[23] = '\0' ;
   }

   /** Load NIFTI specific stuff into the header **/

   if( nim->nifti_type > 0 ){

     if( nim->nifti_type == 1 ) strcpy(nhdr.magic,"n+1") ;   /* 1 file */
     else                       strcpy(nhdr.magic,"ni1") ;   /* 2 files */

     /* for nifti files, enforce positive pixdims! */
     nhdr.pixdim[1] = fabs(nhdr.pixdim[1]) ; nhdr.pixdim[2] = fabs(nhdr.pixdim[2]) ;
     nhdr.pixdim[3] = fabs(nhdr.pixdim[3]) ; nhdr.pixdim[4] = fabs(nhdr.pixdim[4]) ;
     nhdr.pixdim[5] = fabs(nhdr.pixdim[5]) ; nhdr.pixdim[6] = fabs(nhdr.pixdim[6]) ;
     nhdr.pixdim[7] = fabs(nhdr.pixdim[7]) ;

     nhdr.intent_code = nim->intent_code ;
     nhdr.intent_p1   = nim->intent_p1 ;
     nhdr.intent_p2   = nim->intent_p2 ;
     nhdr.intent_p3   = nim->intent_p3 ;
     if( nim->intent_name[0] != '\0' ){
       memcpy(nhdr.intent_name,nim->intent_name,15) ;
       nhdr.intent_name[15] = '\0' ;
     }

     nhdr.vox_offset  = (float) nim->iname_offset ;
     nhdr.xyzt_units  = SPACE_TIME_TO_XYZT( nim->xyz_units, nim->time_units ) ;
     nhdr.toffset     = nim->toffset ;

     if( nim->qform_code > 0 ){
       nhdr.qform_code = nim->qform_code ;
       nhdr.quatern_b  = nim->quatern_b ;
       nhdr.quatern_c  = nim->quatern_c ;
       nhdr.quatern_d  = nim->quatern_d ;
       nhdr.qoffset_x  = nim->qoffset_x ;
       nhdr.qoffset_y  = nim->qoffset_y ;
       nhdr.qoffset_z  = nim->qoffset_z ;
       nhdr.pixdim[0]  = (nim->qfac >= 0.0) ? 1.0 : -1.0 ;
     }

     if( nim->sform_code > 0 ){
       nhdr.sform_code = nim->sform_code ;
       nhdr.srow_x[0]  = nim->sto_xyz.m[0][0] ;
       nhdr.srow_x[1]  = nim->sto_xyz.m[0][1] ;
       nhdr.srow_x[2]  = nim->sto_xyz.m[0][2] ;
       nhdr.srow_x[3]  = nim->sto_xyz.m[0][3] ;
       nhdr.srow_y[0]  = nim->sto_xyz.m[1][0] ;
       nhdr.srow_y[1]  = nim->sto_xyz.m[1][1] ;
       nhdr.srow_y[2]  = nim->sto_xyz.m[1][2] ;
       nhdr.srow_y[3]  = nim->sto_xyz.m[1][3] ;
       nhdr.srow_z[0]  = nim->sto_xyz.m[2][0] ;
       nhdr.srow_z[1]  = nim->sto_xyz.m[2][1] ;
       nhdr.srow_z[2]  = nim->sto_xyz.m[2][2] ;
       nhdr.srow_z[3]  = nim->sto_xyz.m[2][3] ;
     }

     nhdr.dim_info = FPS_INTO_DIM_INFO( nim->freq_dim ,
                                        nim->phase_dim , nim->slice_dim ) ;
     nhdr.slice_code     = nim->slice_code ;
     nhdr.slice_start    = nim->slice_start ;
     nhdr.slice_end      = nim->slice_end ;
     nhdr.slice_duration = nim->slice_duration ;
   }

   return nhdr;
}



int nifti_extension_size(nifti_image *nim)
{
   int c, size = 0;

   if( !nim || nim->num_ext <= 0 ) return 0;

   if( gni_debug > 2 ) fprintf(stderr,"-d ext sizes:");

   for ( c = 0; c < nim->num_ext; c++ ){
      size += nim->ext_list[c].esize;
      if( gni_debug > 2 ) fprintf(stderr,"  %d",nim->ext_list[c].esize);
   }

   if( gni_debug > 2 ) fprintf(stderr," (total = %d)\n",size);

   return size;
}

void nifti_set_iname_offset(nifti_image *nim)
{
   int offset = nifti_extension_size(nim);  /* init based on extensions */

   switch( nim->nifti_type ){

     default:  /* writing into 2 files */
       /* we only write files with 0 offset in the 2 file format */
       nim->iname_offset = 0 ;
     break ;

     case 1:   /* NIFTI-1 single binary file */
       /* need to cast sizeof to int, else -1 may be promoted to unsigned */
       if (nim->iname_offset < (int)sizeof(struct nifti_1_header)) {
	   offset += sizeof(struct nifti_1_header) + 4 ;
	   /* be sure offset is aligned to a 16 byte word boundary */
	   if ( ( offset % 16 ) != 0 )  offset = ((offset + 0xf) & ~0xf);

           if( nim->iname_offset != offset && gni_debug > 1 ){
              fprintf(stderr,"+d changing offset from %d to %d\n",
                      nim->iname_offset, offset);
           }
	   nim->iname_offset = offset;
       }
     break ;

               /* non-standard case: */
     case 3:{  /* NIFTI-1 ASCII header + binary data (single file) */
       nim->iname_offset = -1 ;              /* compute offset from filesize */
     }
   }
}



#undef  ERREX
#define ERREX(msg)                                                \
 do{ fprintf(stderr,"** ERROR: nifti_image_write_hdr_img: %s\n",(msg)) ;  \
     return fp ; } while(0)


/* ----------------------------------------------------------------------
 * This writes the header (and optionally the image data) to file
 * If the image data file is left open it returns a valid znzFile handle
 * It also uses imgfile as the open image file is not null, and modifies
 * it inside.
 * 
 * Values for write_data mode are based on two binary flags
 * ( 0/1 for no-write/write data, and 0/2 for close/leave-open files ) :
 *       0 = do not write data and close (do not open data file)
 *       1 = write data        and close
 *       2 = do not write data and leave data file open 
 *       3 = write data        and leave data file open 
 * ----------------------------------------------------------------------*/

static znzFile nifti_image_write_hdr_img2( nifti_image *nim , int write_data , 
                                           char* opts, znzFile *imgfile,
                                           nifti_brick_list * NBL )
{
   struct nifti_1_header nhdr ;
   znzFile fp=NULL, hdrfile=NULL;
   size_t ss ;

   if( nim        == NULL                         ) ERREX("NULL input") ;
   if( !nifti_validfilename(nim->fname)           ) ERREX("bad fname input") ;
   if( (nim->data == NULL) && ((write_data & 1)!=0) &&
       (NBL == NULL) )                              ERREX("no image data") ;

   nifti_set_iname_offset(nim);

   if( gni_debug > 2 )
      fprintf(stderr,"-d start write of nifti file '%s', type %d, offset %d\n",
              nim->fname ? nim->fname : "<NULL>",
              nim->nifti_type, nim->iname_offset);

   /* make iname from fname, if needed */

   switch( nim->nifti_type ){

     default:  /* writing into 2 files */
       if( nim->iname != NULL && strcmp(nim->iname,nim->fname) == 0 ){
         free(nim->iname) ; nim->iname = NULL ;
       }
       if( nim->iname == NULL ){ 
	 /* choose a new name because either no name set,
          * or it wasn't different from hdr */
	 nim->iname = nifti_findimgname(nim->fname,nim->nifti_type);
         if( nim->iname == NULL ){
            if( gni_debug > 0 )
               fprintf(stderr,"** failed findimgname for '%s'\n", nim->fname);
               return NULL;  
         }
       }
     break ;

     case 1:   /* NIFTI-1 single binary file */
     break ;

               /* non-standard case: */
     case 3:{  /* NIFTI-1 ASCII header + binary data (single file) */
       char *hstr ;
       nim->byteorder = short_order() ;      /* am writing in current order */
       hstr = nifti_image_to_ascii( nim ) ;  /* get header in ASCII form */
       if( hstr == NULL ) ERREX("bad ASCII header creation?") ;
       fp = znzopen( nim->fname , opts , nifti_is_gzfile(nim->fname) ) ;
       if( znz_isnull(fp) ){ free(hstr); ERREX("can't open output file"); }
       znzputs(hstr,fp) ;

       nifti_write_extensions(fp,nim);

       /* writes the binary data to fp */
       if ( (write_data & 1)!=0) { nifti_write_all_data(fp,nim,NBL); }
       if ( (write_data & 2)==0) { znzclose(fp); }
       return fp;  /* returned but may be closed */
     }
   }
     
   /***** Here -- write a binary header *****/
   
   nhdr = nifti_convert_nim2nhdr(nim);

   if (imgfile==NULL) {
     
     /** Open file, write header **/
     
     fp = znzopen( nim->fname , opts , nifti_is_gzfile(nim->fname) ) ;
     if( znz_isnull(fp) ) ERREX("can't open output file") ;
     
     ss = znzwrite( &nhdr , 1 , sizeof(nhdr) , fp ) ;
     if( ss < sizeof(nhdr) ){
       znzclose(fp) ; ERREX("bad write to output file") ;
     }
     nifti_write_extensions(fp,nim);
     
     
     /** if writing data or keeping data file open **/
     if ((write_data & 3)!=0) {
       /** If writing dual files, close header and open image file **/
       if( nim->nifti_type != 1 ){
	 znzclose(fp) ;
	 fp = znzopen( nim->iname , opts , nifti_is_gzfile(nim->iname) ) ;
	 if( znz_isnull(fp) ) ERREX("can't open image file") ;
       }
       /* skip to vox_offset, ready for writing */
       znzseek(fp,nim->iname_offset,SEEK_SET);
       if ( (write_data & 1)!=0)  nifti_write_all_data(fp,nim,NBL);
     }
     
     /** close the file if requested **/
     if ( (write_data & 2)==0) { znzclose(fp); }
     return fp;  /* returned but may be closed */
     
   }

   /** ----- otherwise use imgfile for the image -----**/

   /** write header **/

   if (nim->nifti_type==1) {
     if( znz_isnull(*imgfile) ) ERREX("bad image data file passed in") ;
     /* if header and data file are the same then seek to beginning and write */
     znzseek(*imgfile,0,SEEK_SET);
     ss = znzwrite( &nhdr , 1 , sizeof(nhdr) , *imgfile ) ;
     if( ss < sizeof(nhdr) ){ ERREX("bad write to output file") ; }
     nifti_write_extensions(*imgfile,nim);
   } else {
     /* if a dual file type or no file opened, then start a new file */
     hdrfile = znzopen( nim->fname , opts , nifti_is_gzfile(nim->fname) ) ;
     if( znz_isnull(hdrfile) ) ERREX("can't open output file") ;
     /* if the header file was separate then write and close it now */
     ss = znzwrite( &nhdr , 1 , sizeof(nhdr) , hdrfile ) ;
     if( ss < sizeof(nhdr) ){ znzclose(fp); ERREX("bad write to output file"); }
     nifti_write_extensions(hdrfile,nim);
     znzclose(hdrfile);
   }


   /** if writing data or keeping data file open **/
   if ((write_data & 3)!=0) {  
     /* skip to vox_offset, ready for writing in image file */
     znzseek(*imgfile,nim->iname_offset,SEEK_SET);
     if ( (write_data & 1)!=0) nifti_write_all_data(*imgfile,nim,NBL);
   }

   /** close the file if requested **/
   if ( (write_data & 2)==0) { znzclose(*imgfile); }
   return *imgfile;  /* returned but may be closed */
}


static znzFile nifti_image_write_hdr_img( nifti_image *nim , int write_data , 
                                          char* opts )
{
  return nifti_image_write_hdr_img2(nim,write_data,opts,NULL,NULL);
}


void nifti_image_write( nifti_image *nim )
{
  nifti_image_write_hdr_img(nim,1,"wb");
}

/* new function to pass NBL down to write_all_data */
void nifti_image_write_bricks( nifti_image *nim, nifti_brick_list * NBL )
{
  (void)nifti_image_write_hdr_img2(nim,1,"wb",NULL,NBL);
}



/* copy for nifti_image is via the nifti_1_header structure */
/* it does not copy data */

nifti_image * nifti_copy_nim_info(nifti_image* src)
{
  struct nifti_1_header nhdr;
  nifti_image *dest;
  dest = (nifti_image *)calloc(1,sizeof(nifti_image));
  nhdr = nifti_convert_nim2nhdr(src);
  dest = nifti_convert_nhdr2nim(nhdr,src->fname);
  return dest;
}


/*------------------------------------------------------------------------*/
/* Un-escape a C string in place -- that is, convert XML escape sequences
   back into their characters.  (This can be done in place since the
   replacement is always smaller than the input.)  Escapes recognized are:
     -  &lt;   ->  <
     -  &gt;   ->  >
     -  &quot; ->  "
     -  &apos; ->  '
     -  &amp;  ->  &
   Also replace CR LF pair (Microsoft), or CR alone (Macintosh) with
   LF (Unix), per the XML standard.
   Return value is number of replacements made (if you care).
--------------------------------------------------------------------------*/

#undef  CR
#undef  LF
#define CR 0x0D
#define LF 0x0A

int unescape_string( char *str )
{
   int ii,jj , nn,ll ;

   if( str == NULL ) return 0 ;                /* no string? */
   ll = strlen(str) ; if( ll == 0 ) return 0 ;

   /* scan for escapes: &something; */

   for( ii=jj=nn=0 ; ii<ll ; ii++,jj++ ){ /* scan at ii; results go in at jj */

     if( str[ii] == '&' ){  /* start of escape? */

             if( ii+3 < ll        &&   /* &lt; */
                 str[ii+1] == 'l' &&
                 str[ii+2] == 't' &&
                 str[ii+3] == ';'   ){ str[jj] = '<' ; ii += 3 ; nn++ ; }

        else if( ii+3 < ll        &&   /* &gt; */
                 str[ii+1] == 'g' &&
                 str[ii+2] == 't' &&
                 str[ii+3] == ';'   ){ str[jj] = '>' ; ii += 3 ; nn++ ; }

        else if( ii+5 < ll        &&   /* &quot; */
                 str[ii+1] == 'q' &&
                 str[ii+2] == 'u' &&
                 str[ii+3] == 'o' &&
                 str[ii+4] == 't' &&
                 str[ii+5] == ';'   ){ str[jj] = '"' ; ii += 5 ; nn++ ; }

        else if( ii+5 < ll        &&   /* &apos; */
                 str[ii+1] == 'a' &&
                 str[ii+2] == 'p' &&
                 str[ii+3] == 'o' &&
                 str[ii+4] == 's' &&
                 str[ii+5] == ';'   ){ str[jj] = '\'' ; ii += 5 ; nn++ ; }

        else if( ii+4 < ll        &&  /* &amp; */
                 str[ii+1] == 'a' &&
                 str[ii+2] == 'm' &&
                 str[ii+3] == 'p' &&
                 str[ii+4] == ';'   ){ str[jj] = '&' ; ii += 4 ; nn++ ; }

        /* although the comments above don't mention it,
           we also look for XML style numeric escapes
           of the forms &#32; (decimal) and &#xfd; (hex) */

        else if( ii+3 < ll        &&
                 str[ii+1] == '#' &&
                 isdigit(str[ii+2]) ){   /* &#dec; */

           unsigned int val='?' ; int kk=ii+3 ;
           while( kk < ll && kk != ';' ) kk++ ;
           sscanf( str+ii+2 , "%u" , &val ) ;
           str[jj] = (char) val ; ii = kk ; nn++ ;
        }

        else if( ii+4 < ll        &&
                 str[ii+1] == '#' &&
                 str[ii+2] == 'x' &&
                 isxdigit(str[ii+3]) ){   /* &#hex; */

           unsigned int val='?' ; int kk=ii+4 ;
           while( kk < ll && kk != ';' ) kk++ ;
           sscanf( str+ii+3 , "%x" , &val ) ;
           str[jj] = (char) val ; ii = kk ; nn++ ;
        }

        /* didn't start a recognized escape, so just copy as normal */

        else if( jj < ii ){ str[jj] = str[ii] ; }

     } else if( str[ii] == CR ) {  /* is a carriage return */

        if( str[ii+1] == LF ){ str[jj] = LF ; ii++ ; nn++ ; }  /* CR LF */
        else                 { str[jj] = LF ;      ; nn++ ; }  /* CR only */

     } else { /* is a normal character, just copy to output */

             if( jj < ii ){ str[jj] = str[ii] ; }
     }

     /* at this point, ii=index of last character used up in scan
                       jj=index of last character written to (jj <= ii) */
   }

   if( jj < ll ) str[jj] = '\0' ; /* end string properly */

   return nn ;
}

/*------------------------------------------------------------------------*/
/* Quotize (and escapize) one string, returning a new string.
   Approximately speaking, this is the inverse of unescape_string().
   The result should be free()-ed when you are done with it.
--------------------------------------------------------------------------*/

char *escapize_string( char *str )
{
   int ii,jj , lstr,lout ;
   char *out ;

   if( str == NULL || (lstr=strlen(str)) == 0 ){      /* 0 length */
     out = nifti_strdup("''") ; return out ;                /* string?? */
   }

   lout = 4 ;                      /* initialize length of output */
   for( ii=0 ; ii < lstr ; ii++ ){ /* count characters for output */
     switch( str[ii] ){
       case '&':  lout += 5 ; break ;  /* replace '&' with "&amp;" */

       case '<':
       case '>':  lout += 4 ; break ;  /* replace '<' with "&lt;" */

       case '"' :
       case '\'': lout += 6 ; break ;  /* replace '"' with "&quot;" */

       case CR:
       case LF:   lout += 6 ; break ;  /* replace CR with "&#x0d;"
                                                  LF with "&#x0a;" */

       default: lout++ ; break ;      /* copy all other chars */
     }
   }
   out = (char *)calloc(1,lout) ;     /* allocate output string */
   out[0] = '\'' ;                    /* opening quote mark */
   for( ii=0,jj=1 ; ii < lstr ; ii++ ){
      switch( str[ii] ){
         default: out[jj++] = str[ii] ; break ;  /* normal characters */

         case '&':  memcpy(out+jj,"&amp;",5)  ; jj+=5 ; break ;

         case '<':  memcpy(out+jj,"&lt;",4)   ; jj+=4 ; break ;
         case '>':  memcpy(out+jj,"&gt;",4)   ; jj+=4 ; break ;

         case '"' : memcpy(out+jj,"&quot;",6) ; jj+=6 ; break ;

         case '\'': memcpy(out+jj,"&apos;",6) ; jj+=6 ; break ;

         case CR:   memcpy(out+jj,"&#x0d;",6) ; jj+=6 ; break ;
         case LF:   memcpy(out+jj,"&#x0a;",6) ; jj+=6 ; break ;
      }
   }
   out[jj++] = '\''  ;  /* closing quote mark */
   out[jj]   = '\0' ;  /* terminate the string */
   return out ;
}

/*---------------------------------------------------------------------------*/
/* Dump the information in a NIFTI image header to an XML-ish ASCII string
   that can later be converted back into a NIFTI header in
   nifti_image_from_ascii().  The resulting string can be free()-ed when
   you are done with it.
-----------------------------------------------------------------------------*/

char *nifti_image_to_ascii( nifti_image *nim )
{
   char *buf , *ebuf ; int nbuf ;

   if( nim == NULL ) return NULL ;   /* stupid caller */

   buf = (char *)calloc(1,65534); nbuf = 0;  /* longer than needed, to be safe */

   sprintf( buf , "<nifti_image\n" ) ;   /* XML-ish opener */

   sprintf( buf+strlen(buf) , "  nifti_type = '%s'\n" ,
              (nim->nifti_type == 1) ? "NIFTI-1+"
             :(nim->nifti_type == 2) ? "NIFTI-1"
             :(nim->nifti_type == 3) ? "NIFTI-1A"
             :                         "ANALYZE-7.5" ) ;

   /** Strings that we don't control (filenames, etc.) that might
       contain "weird" characters (like quotes) are "escaped":
       - A few special characters are replaced by XML-style escapes, using
         the function escapize_string().
       - On input, function unescape_string() reverses this process.
       - The result is that the NIFTI ASCII-format header is XML-compliant. **/

   ebuf = escapize_string(nim->fname) ;
   sprintf( buf+strlen(buf) , "  header_filename = %s\n",ebuf); free(ebuf);

   ebuf = escapize_string(nim->iname) ;
   sprintf( buf+strlen(buf) , "  image_filename = %s\n", ebuf); free(ebuf);

   sprintf( buf+strlen(buf) , "  image_offset = '%d'\n" , nim->iname_offset );

                       sprintf( buf+strlen(buf), "  ndim = '%d'\n", nim->ndim);
                       sprintf( buf+strlen(buf), "  nx = '%d'\n",   nim->nx  );
   if( nim->ndim > 1 ) sprintf( buf+strlen(buf), "  ny = '%d'\n",   nim->ny  );
   if( nim->ndim > 2 ) sprintf( buf+strlen(buf), "  nz = '%d'\n",   nim->nz  );
   if( nim->ndim > 3 ) sprintf( buf+strlen(buf), "  nt = '%d'\n",   nim->nt  );
   if( nim->ndim > 4 ) sprintf( buf+strlen(buf), "  nu = '%d'\n",   nim->nu  );
   if( nim->ndim > 5 ) sprintf( buf+strlen(buf), "  nv = '%d'\n",   nim->nv  );
   if( nim->ndim > 6 ) sprintf( buf+strlen(buf), "  nw = '%d'\n",   nim->nw  );
                       sprintf( buf+strlen(buf), "  dx = '%g'\n",   nim->dx  );
   if( nim->ndim > 1 ) sprintf( buf+strlen(buf), "  dy = '%g'\n",   nim->dy  );
   if( nim->ndim > 2 ) sprintf( buf+strlen(buf), "  dz = '%g'\n",   nim->dz  );
   if( nim->ndim > 3 ) sprintf( buf+strlen(buf), "  dt = '%g'\n",   nim->dt  );
   if( nim->ndim > 4 ) sprintf( buf+strlen(buf), "  du = '%g'\n",   nim->du  );
   if( nim->ndim > 5 ) sprintf( buf+strlen(buf), "  dv = '%g'\n",   nim->dv  );
   if( nim->ndim > 6 ) sprintf( buf+strlen(buf), "  dw = '%g'\n",   nim->dw  );

   sprintf( buf+strlen(buf) , "  datatype = '%d'\n" , nim->datatype ) ;
   sprintf( buf+strlen(buf) , "  datatype_name = '%s'\n" ,
                              nifti_datatype_string(nim->datatype) ) ;

   sprintf( buf+strlen(buf) , "  nvox = '%d'\n" , nim->nvox ) ;
   sprintf( buf+strlen(buf) , "  nbyper = '%d'\n" , nim->nbyper ) ;

   sprintf( buf+strlen(buf) , "  byteorder = '%s'\n" ,
            (nim->byteorder==MSB_FIRST) ? "MSB_FIRST" : "LSB_FIRST" ) ;

   if( nim->cal_min < nim->cal_max ){
     sprintf( buf+strlen(buf) , "  cal_min = '%g'\n", nim->cal_min ) ;
     sprintf( buf+strlen(buf) , "  cal_max = '%g'\n", nim->cal_max ) ;
   }

   if( nim->scl_slope != 0.0 ){
     sprintf( buf+strlen(buf) , "  scl_slope = '%g'\n" , nim->scl_slope ) ;
     sprintf( buf+strlen(buf) , "  scl_inter = '%g'\n" , nim->scl_inter ) ;
   }

   if( nim->intent_code > 0 ){
     sprintf( buf+strlen(buf) , "  intent_code = '%d'\n", nim->intent_code ) ;
     sprintf( buf+strlen(buf) , "  intent_code_name = '%s'\n" ,
                                nifti_intent_string(nim->intent_code) ) ;
     sprintf( buf+strlen(buf) , "  intent_p1 = '%g'\n" , nim->intent_p1 ) ;
     sprintf( buf+strlen(buf) , "  intent_p2 = '%g'\n" , nim->intent_p2 ) ;
     sprintf( buf+strlen(buf) , "  intent_p3 = '%g'\n" , nim->intent_p3 ) ;

     if( nim->intent_name[0] != '\0' ){
       ebuf = escapize_string(nim->intent_name) ;
       sprintf( buf+strlen(buf) , "  intent_name = %s\n",ebuf) ;
       free(ebuf) ;
     }
   }

   if( nim->toffset != 0.0 )
     sprintf( buf+strlen(buf) , "  toffset = '%g'\n",nim->toffset ) ;

   if( nim->xyz_units > 0 )
     sprintf( buf+strlen(buf) ,
              "  xyz_units = '%d'\n"
              "  xyz_units_name = '%s'\n" ,
              nim->xyz_units , nifti_units_string(nim->xyz_units) ) ;

   if( nim->time_units > 0 )
     sprintf( buf+strlen(buf) ,
              "  time_units = '%d'\n"
              "  time_units_name = '%s'\n" ,
              nim->time_units , nifti_units_string(nim->time_units) ) ;

   if( nim->freq_dim > 0 )
     sprintf( buf+strlen(buf) , "  freq_dim = '%d'\n",nim->freq_dim ) ;
   if( nim->phase_dim > 0 )
     sprintf( buf+strlen(buf) , "  phase_dim = '%d'\n",nim->phase_dim ) ;
   if( nim->slice_dim > 0 )
     sprintf( buf+strlen(buf) , "  slice_dim = '%d'\n",nim->slice_dim ) ;
   if( nim->slice_code > 0 )
     sprintf( buf+strlen(buf) ,
              "  slice_code = '%d'\n"
              "  slice_code_name = '%s'\n" ,
              nim->slice_code , nifti_slice_string(nim->slice_code) ) ;
   if( nim->slice_start >= 0 && nim->slice_end > nim->slice_start )
     sprintf( buf+strlen(buf) ,
              "  slice_start = '%d'\n"
              "  slice_end = '%d'\n"  , nim->slice_start , nim->slice_end ) ;
   if( nim->slice_duration != 0.0 )
     sprintf( buf+strlen(buf) , "  slice_duration = '%g'\n",
              nim->slice_duration ) ;

   if( nim->descrip[0] != '\0' ){
     ebuf = escapize_string(nim->descrip) ;
     sprintf( buf+strlen(buf) , "  descrip = %s\n",ebuf) ;
     free(ebuf) ;
   }

   if( nim->aux_file[0] != '\0' ){
     ebuf = escapize_string(nim->aux_file) ;
     sprintf( buf+strlen(buf) , "  aux_file = %s\n",ebuf) ;
     free(ebuf) ;
   }

   if( nim->qform_code > 0 ){
     int i,j,k ;

     sprintf( buf+strlen(buf) ,
              "  qform_code = '%d'\n"
              "  qform_code_name = '%s'\n"
     "  qto_xyz_matrix = '%g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g'\n" ,
         nim->qform_code      , nifti_xform_string(nim->qform_code) ,
         nim->qto_xyz.m[0][0] , nim->qto_xyz.m[0][1] ,
         nim->qto_xyz.m[0][2] , nim->qto_xyz.m[0][3] ,
         nim->qto_xyz.m[1][0] , nim->qto_xyz.m[1][1] ,
         nim->qto_xyz.m[1][2] , nim->qto_xyz.m[1][3] ,
         nim->qto_xyz.m[2][0] , nim->qto_xyz.m[2][1] ,
         nim->qto_xyz.m[2][2] , nim->qto_xyz.m[2][3] ,
         nim->qto_xyz.m[3][0] , nim->qto_xyz.m[3][1] ,
         nim->qto_xyz.m[3][2] , nim->qto_xyz.m[3][3]  ) ;

     sprintf( buf+strlen(buf) ,
     "  qto_ijk_matrix = '%g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g'\n" ,
         nim->qto_ijk.m[0][0] , nim->qto_ijk.m[0][1] ,
         nim->qto_ijk.m[0][2] , nim->qto_ijk.m[0][3] ,
         nim->qto_ijk.m[1][0] , nim->qto_ijk.m[1][1] ,
         nim->qto_ijk.m[1][2] , nim->qto_ijk.m[1][3] ,
         nim->qto_ijk.m[2][0] , nim->qto_ijk.m[2][1] ,
         nim->qto_ijk.m[2][2] , nim->qto_ijk.m[2][3] ,
         nim->qto_ijk.m[3][0] , nim->qto_ijk.m[3][1] ,
         nim->qto_ijk.m[3][2] , nim->qto_ijk.m[3][3]  ) ;

     sprintf( buf+strlen(buf) ,
              "  quatern_b = '%g'\n"
              "  quatern_c = '%g'\n"
              "  quatern_d = '%g'\n"
              "  qoffset_x = '%g'\n"
              "  qoffset_y = '%g'\n"
              "  qoffset_z = '%g'\n"
              "  qfac = '%g'\n" ,
         nim->quatern_b , nim->quatern_c , nim->quatern_d ,
         nim->qoffset_x , nim->qoffset_y , nim->qoffset_z , nim->qfac ) ;

     mat44_to_orientation( nim->qto_xyz , &i,&j,&k ) ;
     if( i > 0 && j > 0 && k > 0 )
       sprintf( buf+strlen(buf) ,
                "  qform_i_orientation = '%s'\n"
                "  qform_j_orientation = '%s'\n"
                "  qform_k_orientation = '%s'\n" ,
                nifti_orientation_string(i) ,
                nifti_orientation_string(j) ,
                nifti_orientation_string(k)  ) ;
   }

   if( nim->sform_code > 0 ){
     int i,j,k ;

     sprintf( buf+strlen(buf) ,
              "  sform_code = '%d'\n"
              "  sform_code_name = '%s'\n"
     "  sto_xyz_matrix = '%g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g'\n" ,
         nim->sform_code      , nifti_xform_string(nim->sform_code) ,
         nim->sto_xyz.m[0][0] , nim->sto_xyz.m[0][1] ,
         nim->sto_xyz.m[0][2] , nim->sto_xyz.m[0][3] ,
         nim->sto_xyz.m[1][0] , nim->sto_xyz.m[1][1] ,
         nim->sto_xyz.m[1][2] , nim->sto_xyz.m[1][3] ,
         nim->sto_xyz.m[2][0] , nim->sto_xyz.m[2][1] ,
         nim->sto_xyz.m[2][2] , nim->sto_xyz.m[2][3] ,
         nim->sto_xyz.m[3][0] , nim->sto_xyz.m[3][1] ,
         nim->sto_xyz.m[3][2] , nim->sto_xyz.m[3][3]  ) ;

     sprintf( buf+strlen(buf) ,
     "  sto_ijk matrix = '%g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g'\n" ,
         nim->sto_ijk.m[0][0] , nim->sto_ijk.m[0][1] ,
         nim->sto_ijk.m[0][2] , nim->sto_ijk.m[0][3] ,
         nim->sto_ijk.m[1][0] , nim->sto_ijk.m[1][1] ,
         nim->sto_ijk.m[1][2] , nim->sto_ijk.m[1][3] ,
         nim->sto_ijk.m[2][0] , nim->sto_ijk.m[2][1] ,
         nim->sto_ijk.m[2][2] , nim->sto_ijk.m[2][3] ,
         nim->sto_ijk.m[3][0] , nim->sto_ijk.m[3][1] ,
         nim->sto_ijk.m[3][2] , nim->sto_ijk.m[3][3]  ) ;

     mat44_to_orientation( nim->sto_xyz , &i,&j,&k ) ;
     if( i > 0 && j > 0 && k > 0 )
       sprintf( buf+strlen(buf) ,
                "  sform_i_orientation = '%s'\n"
                "  sform_j_orientation = '%s'\n"
                "  sform_k_orientation = '%s'\n" ,
                nifti_orientation_string(i) ,
                nifti_orientation_string(j) ,
                nifti_orientation_string(k)  ) ;
   }

   sprintf( buf+strlen(buf) , "  num_ext = '%d'\n", nim->num_ext ) ;

   sprintf( buf+strlen(buf) , "/>\n" ) ;   /* XML-ish closer */

   nbuf = strlen(buf) ;
   buf  = (char *)realloc((void *)buf, nbuf+1); /* cut back to proper length */
   return buf ;
}

/*---------------------------------------------------------------------------*/

int short_order(void)   /* determine this CPU's byte order */
{
   union { unsigned char bb[2] ;
           short         ss    ; } fred ;

   fred.bb[0] = 1 ; fred.bb[1] = 0 ;

   return (fred.ss == 1) ? LSB_FIRST : MSB_FIRST ;
}

/*---------------------------------------------------------------------------*/

#undef  QQNUM
#undef  QNUM
#undef  QSTR

/* macro to check lhs string against "n1"; if it matches,
   interpret rhs string as a number, and put it into nim->"n2" */

#define QQNUM(n1,n2) if( strcmp(lhs,#n1)==0 ) nim->n2=strtod(rhs,NULL)

/* same, but where "n1" == "n2" */

#define QNUM(nam)    QQNUM(nam,nam)

/* macro to check lhs string against "nam"; if it matches,
   put rhs string into nim->"nam" string, with max length = "ml" */

#define QSTR(nam,ml) if( strcmp(lhs,#nam) == 0 )                           \
                       strncpy(nim->nam,rhs,ml), nim->intent_name[ml]='\0'

/*---------------------------------------------------------------------------*/
/* Take an XML-ish ASCII string and create a NIFTI image header to match.
   NULL is returned if enough information isn't present in the input string.
    - The image data can later be loaded with nifti_image_load().
    - The struct returned here can be liberated with nifti_image_free().
    - Not a lot of error checking is done here to make sure that the
      input values are reasonable!
-----------------------------------------------------------------------------*/

nifti_image *nifti_image_from_ascii( char *str, int * bytes_read )
{
   char lhs[1024] , rhs[1024] ;
   int ii , spos, nn , slen ;
   nifti_image *nim ;              /* will be output */

   if( str == NULL || *str == '\0' ) return NULL ;  /* bad input!? */

   /* scan for opening string */

   spos = 0 ; slen = strlen(str) ;
   ii = sscanf( str+spos , "%1023s%n" , lhs , &nn ) ; spos += nn ;
   if( ii == 0 || strcmp(lhs,"<nifti_image") != 0 ) return NULL ;

   /* create empty image struct */

   nim = (nifti_image *) calloc( 1 , sizeof(nifti_image) ) ;

   nim->nx = nim->ny = nim->nz = nim->nt
           = nim->nu = nim->nv = nim->nw = 1 ;
   nim->dx = nim->dy = nim->dz = nim->dt
           = nim->du = nim->dv = nim->dw = nim->qfac = 1.0 ;

   nim->byteorder = short_order() ;

   /* starting at str[spos], scan for "equations" of the form
         lhs = 'rhs'
      and assign rhs values into the struct component named by lhs */

   while(1){

     while( isspace(str[spos]) ) spos++ ;  /* skip whitespace */
     if( str[spos] == '\0' ) break ;       /* end of string? */

     /* get lhs string */

     ii = sscanf( str+spos , "%1023s%n" , lhs , &nn ) ; spos += nn ;
     if( ii == 0 || strcmp(lhs,"/>") == 0 ) break ;  /* end of input? */

     /* skip whitespace and the '=' marker */

     while( isspace(str[spos]) || str[spos] == '=' ) spos++ ;
     if( str[spos] == '\0' ) break ;       /* end of string? */

     /* if next character is a quote ', copy everything up to next '
        otherwise, copy everything up to next nonblank              */

     if( str[spos] == '\'' ){
        ii = spos+1 ;
        while( str[ii] != '\0' && str[ii] != '\'' ) ii++ ;
        nn = ii-spos-1 ; if( nn > 1023 ) nn = 1023 ;
        memcpy(rhs,str+spos+1,nn) ; rhs[nn] = '\0' ;
        spos = (str[ii] == '\'') ? ii+1 : ii ;
     } else {
        ii = sscanf( str+spos , "%1023s%n" , rhs , &nn ) ; spos += nn ;
        if( ii == 0 ) break ;  /* nothing found? */
     }
     unescape_string(rhs) ;  /* remove any XML escape sequences */

     /* Now can do the assignment, based on lhs string.
        Start with special cases that don't fit the QNUM/QSTR macros. */

     if( strcmp(lhs,"nifti_type") == 0 ){
            if( strcmp(rhs,"ANALYZE-7.5") == 0 ) nim->nifti_type = 0 ;
       else if( strcmp(rhs,"NIFTI-1+")    == 0 ) nim->nifti_type = 1 ;
       else if( strcmp(rhs,"NIFTI-1")     == 0 ) nim->nifti_type = 2 ;
       else if( strcmp(rhs,"NIFTI-1A")    == 0 ) nim->nifti_type = 3 ;
     }
     else if( strcmp(lhs,"header_filename") == 0 ){
       nim->fname = nifti_strdup(rhs) ;
     }
     else if( strcmp(lhs,"image_filename") == 0 ){
       nim->iname = nifti_strdup(rhs) ;
     }
     else if( strcmp(lhs,"sto_xyz_matrix") == 0 ){
       sscanf( rhs , "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f" ,
               &(nim->sto_xyz.m[0][0]) , &(nim->sto_xyz.m[0][1]) ,
               &(nim->sto_xyz.m[0][2]) , &(nim->sto_xyz.m[0][3]) ,
               &(nim->sto_xyz.m[1][0]) , &(nim->sto_xyz.m[1][1]) ,
               &(nim->sto_xyz.m[1][2]) , &(nim->sto_xyz.m[1][3]) ,
               &(nim->sto_xyz.m[2][0]) , &(nim->sto_xyz.m[2][1]) ,
               &(nim->sto_xyz.m[2][2]) , &(nim->sto_xyz.m[2][3]) ,
               &(nim->sto_xyz.m[3][0]) , &(nim->sto_xyz.m[3][1]) ,
               &(nim->sto_xyz.m[3][2]) , &(nim->sto_xyz.m[3][3])  ) ;
     }
     else if( strcmp(lhs,"byteorder") == 0 ){
       if( strcmp(rhs,"MSB_FIRST") == 0 ) nim->byteorder = MSB_FIRST ;
       if( strcmp(rhs,"LSB_FIRST") == 0 ) nim->byteorder = LSB_FIRST ;
     }
     else QQNUM(image_offset,iname_offset) ;
     else QNUM(datatype) ;
     else QNUM(ndim) ;
     else QNUM(nx) ;
     else QNUM(ny) ;
     else QNUM(nz) ;
     else QNUM(nt) ;
     else QNUM(nu) ;
     else QNUM(nv) ;
     else QNUM(nw) ;
     else QNUM(dx) ;
     else QNUM(dy) ;
     else QNUM(dz) ;
     else QNUM(dt) ;
     else QNUM(du) ;
     else QNUM(dv) ;
     else QNUM(dw) ;
     else QNUM(cal_min) ;
     else QNUM(cal_max) ;
     else QNUM(scl_slope) ;
     else QNUM(scl_inter) ;
     else QNUM(intent_code) ;
     else QNUM(intent_p1) ;
     else QNUM(intent_p2) ;
     else QNUM(intent_p3) ;
     else QSTR(intent_name,15) ;
     else QNUM(toffset) ;
     else QNUM(xyz_units) ;
     else QNUM(time_units) ;
     else QSTR(descrip,79) ;
     else QSTR(aux_file,23) ;
     else QNUM(qform_code) ;
     else QNUM(quatern_b) ;
     else QNUM(quatern_c) ;
     else QNUM(quatern_d) ;
     else QNUM(qoffset_x) ;
     else QNUM(qoffset_y) ;
     else QNUM(qoffset_z) ;
     else QNUM(qfac) ;
     else QNUM(sform_code) ;
     else QNUM(freq_dim) ;
     else QNUM(phase_dim) ;
     else QNUM(slice_dim) ;
     else QNUM(slice_code) ;
     else QNUM(slice_start) ;
     else QNUM(slice_end) ;
     else QNUM(slice_duration) ;
     else QNUM(num_ext) ;

   } /* end of while loop */

   if( bytes_read ) *bytes_read = spos+1;         /* "process" last '\n' */

   /** do miscellaneous checking and cleanup **/

   if( nim->ndim <= 0 ){ nifti_image_free(nim); return NULL; } /** bad! **/

   nifti_datatype_sizes( nim->datatype, &(nim->nbyper), &(nim->swapsize) );
   if( nim->nbyper == 0 ){ nifti_image_free(nim); return NULL; } /** bad! **/

   nim->dim[0] = nim->ndim ;
   nim->dim[1] = nim->nx ; nim->pixdim[1] = nim->dx ;
   nim->dim[2] = nim->ny ; nim->pixdim[2] = nim->dy ;
   nim->dim[3] = nim->nz ; nim->pixdim[3] = nim->dz ;
   nim->dim[4] = nim->nt ; nim->pixdim[4] = nim->dt ;
   nim->dim[5] = nim->nu ; nim->pixdim[5] = nim->du ;
   nim->dim[6] = nim->nv ; nim->pixdim[6] = nim->dv ;
   nim->dim[7] = nim->nw ; nim->pixdim[7] = nim->dw ;

   nim->nvox =  nim->nx * nim->ny * nim->nz
              * nim->nt * nim->nu * nim->nv * nim->nw ;

   if( nim->qform_code > 0 )
     nim->qto_xyz = quatern_to_mat44(
                      nim->quatern_b, nim->quatern_c, nim->quatern_d,
                      nim->qoffset_x, nim->qoffset_y, nim->qoffset_z,
                      nim->dx       , nim->dy       , nim->dz       ,
                      nim->qfac                                      ) ;
   else
     nim->qto_xyz = quatern_to_mat44(
                      0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 ,
                      nim->dx , nim->dy , nim->dz , 0.0 ) ;


   nim->qto_ijk = mat44_inverse( nim->qto_xyz ) ;

   if( nim->sform_code > 0 )
     nim->sto_ijk = mat44_inverse( nim->sto_xyz ) ;

   return nim ;
}