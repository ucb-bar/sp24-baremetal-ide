/*
  rnn.c
  Recurrent Neural Network in C.

  $ cc -o rnn rnn.c -lm
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define DEBUG_LAYER 0


/* f: input generator */
static int f(int i)
{
    static int a[] = { 5, 9, 4, 0, 5, 9, 6, 3 };
    return a[i % 8];
}
/* g: function to learn */
static int g(int i)
{
    return ((i % 8) == 4)? 1 : 0;
}


/*  Misc. functions
 */

/* rnd(): uniform random [0.0, 1.0] */
static inline double rnd()
{
    return ((double)rand() / RAND_MAX);
}

/* nrnd(): normal random (std=1.0) */
static inline double nrnd()
{
    return (rnd()+rnd()+rnd()+rnd()-2.0) * 1.724; /* std=1.0 */
}

#if 0
/* tanh(x): hyperbolic tangent */
static inline double tanh(double x)
{
    return 2.0 / (1.0 + exp(-2*x)) - 1.0;
}
#endif
/* tanh_g(y): hyperbolic tangent gradient */
static inline double tanh_g(double y)
{
    return 1.0 - y*y;
}


/*  RNNLayer
 */

typedef struct _RNNLayer {

    int lid;                    /* Layer ID */
    struct _RNNLayer* lprev;    /* Previous Layer */
    struct _RNNLayer* lnext;    /* Next Layer */

    int nnodes;                 /* Num. of Nodes */
    int ntimes;                 /* Num. of Times */

    /* array layout: [ v[t=0], v[t=1], ..., v[t=ntimes-1] ] */
    double* outputs;            /* Node Outputs (Hidden) */
    double* errors;             /* Node Errors */

    int nxweights;               /* Num. of XWeights */
    double* xweights;            /* XWeights (trained) */
    double* u_xweights;          /* XWeight Updates */
    int nhweights;               /* Num. of HWeights */
    double* hweights;            /* HWeights (trained) */
    double* u_hweights;          /* HWeight Updates */

    int nbiases;                /* Num. of Biases */
    double* biases;             /* Biases (trained) */
    double* u_biases;           /* Bias Updates */

} RNNLayer;

/* RNNLayer_create(lprev, nnodes)
   Creates a RNNLayer object.
*/
RNNLayer* RNNLayer_create(RNNLayer* lprev, int nnodes, int ntimes)
{
    RNNLayer* self = (RNNLayer*)calloc(1, sizeof(RNNLayer));
    if (self == NULL) return NULL;

    self->lprev = lprev;
    self->lnext = NULL;
    self->lid = 0;
    if (lprev != NULL) {
        assert (lprev->lnext == NULL);
        lprev->lnext = self;
        self->lid = lprev->lid+1;
    }

    self->nnodes = nnodes;
    self->ntimes = ntimes;
    int n = self->nnodes * self->ntimes;
    self->outputs = (double*)calloc(n, sizeof(double));
    self->errors = (double*)calloc(n, sizeof(double));

    if (lprev != NULL) {
        /* Fully connected */
        self->nxweights = lprev->nnodes * self->nnodes;
        self->xweights = (double*)calloc(self->nxweights, sizeof(double));
        self->u_xweights = (double*)calloc(self->nxweights, sizeof(double));
        for (int i = 0; i < self->nxweights; i++) {
            self->xweights[i] = 0.1 * nrnd();
        }
        self->nhweights = self->nnodes * self->nnodes;
        self->hweights = (double*)calloc(self->nhweights, sizeof(double));
        self->u_hweights = (double*)calloc(self->nhweights, sizeof(double));
        for (int i = 0; i < self->nhweights; i++) {
            self->hweights[i] = 0.1 * nrnd();
        }

        self->nbiases = self->nnodes;
        self->biases = (double*)calloc(self->nbiases, sizeof(double));
        self->u_biases = (double*)calloc(self->nbiases, sizeof(double));
        for (int i = 0; i < self->nbiases; i++) {
            self->biases[i] = 0;
        }
    }

    return self;
}

/* RNNLayer_destroy(self)
   Releases the memory.
*/
void RNNLayer_destroy(RNNLayer* self)
{
    assert (self != NULL);

    free(self->outputs);
    free(self->errors);

    if (self->xweights != NULL) {
        free(self->xweights);
    }
    if (self->u_xweights != NULL) {
        free(self->u_xweights);
    }
    if (self->hweights != NULL) {
        free(self->hweights);
    }
    if (self->u_hweights != NULL) {
        free(self->u_hweights);
    }

    if (self->biases != NULL) {
        free(self->biases);
    }
    if (self->u_biases != NULL) {
        free(self->u_biases);
    }

    free(self);
}

/* RNNLayer_dump(self, fp)
   Shows the debug output.
*/
void RNNLayer_dump(const RNNLayer* self, FILE* fp)
{
    assert (self != NULL);
    RNNLayer* lprev = self->lprev;
    fprintf(fp, "RNNLayer%d", self->lid);
    if (lprev != NULL) {
        fprintf(fp, " (<- Layer%d)", lprev->lid);
    }
    fprintf(fp, ": nodes=%d\n", self->nnodes);

    int k = 0;
    for (int t = 0; t < self->ntimes; t++) {
        fprintf(fp, "  outputs(%d) = [", k);
        for (int i = 0; i < self->nnodes; i++) {
            fprintf(fp, " %.4f", self->outputs[k++]);
        }
        fprintf(fp, "]\n");
    }

    if (self->xweights != NULL) {
        fprintf(fp, "  xweights = [");
        for (int i = 0; i < self->nxweights; i++) {
            fprintf(fp, " %.4f", self->xweights[i]);
        }
        fprintf(fp, "]\n");
    }
    if (self->hweights != NULL) {
        fprintf(fp, "  hweights = [");
        for (int i = 0; i < self->nhweights; i++) {
            fprintf(fp, " %.4f", self->hweights[i]);
        }
        fprintf(fp, "]\n");
    }

    if (self->biases != NULL) {
        fprintf(fp, "  biases = [");
        for (int i = 0; i < self->nbiases; i++) {
            fprintf(fp, " %.4f", self->biases[i]);
        }
        fprintf(fp, "]\n");
    }
}


void RNNLayer_reset(RNNLayer* self)
{
    assert (self != NULL);

    for (int i = 0; i < self->nnodes; i++) {
        self->outputs[i] = 0;
    }
}


/* RNNLayer_feedForw(self)
   Performs feed forward updates.
*/
static void RNNLayer_feedForw(RNNLayer* self)
{
    assert (self->lprev != NULL);
    RNNLayer* lprev = self->lprev;

    /* Save the previous values. */
    for (int t = self->ntimes-1; 0 < t; t--) {
        int idst = self->nnodes * t;
        int isrc = self->nnodes * (t-1);
        for (int i = 0; i < self->nnodes; i++) {
            self->outputs[idst + i] = self->outputs[isrc + i];
            self->errors[idst + i] = self->errors[isrc + i];
        }
    }
    /* outputs[0..] and errors[0..] are now replaced by the new values. */

    int kx = 0, kh = 0;
    for (int i = 0; i < self->nnodes; i++) {
        /* H = f(Bh + Wx * X + Wh * H) */
        double h = self->biases[i];
        for (int j = 0; j < lprev->nnodes; j++) {
            h += (lprev->outputs[j] * self->xweights[kx++]);
        }
        for (int j = 0; j < self->nnodes; j++) {
            h += (self->outputs[j] * self->hweights[kh++]);
        }
        self->outputs[i] = tanh(h);
    }

#if DEBUG_LAYER
    fprintf(stderr, "RNNLayer_feedForw(Layer%d):\n", self->lid);
    fprintf(stderr, "  outputs = [");
    for (int i = 0; i < self->nnodes; i++) {
        fprintf(stderr, " %.4f", self->outputs[i]);
    }
    fprintf(stderr, "]\n");
#endif
}

/* RNNLayer_feedBack(self)
   Performs backpropagation.
*/
static void RNNLayer_feedBack(RNNLayer* self)
{
    if (self->lprev == NULL) return;

    assert (self->lprev != NULL);
    RNNLayer* lprev = self->lprev;

    /* Clear errors. */
    for (int j = 0; j < lprev->nnodes; j++) {
        lprev->errors[j] = 0;
    }

    int k = 0;
    for (int t = 0; t < self->ntimes; t++) {
        int kx = 0, kh = 0;
        for (int i = 0; i < self->nnodes; i++) {
            /* Computer the weight/bias updates. */
            double y = self->outputs[k];
            double g = tanh_g(y);
            double dnet = self->errors[k] * g;
            for (int j = 0; j < lprev->nnodes; j++) {
                /* Propagate the errors to the previous layer. */
                lprev->errors[j] += self->xweights[kx] * dnet;
                self->u_xweights[kx] += dnet * lprev->outputs[j];
                kx++;
            }
            for (int j = 0; j < self->nnodes; j++) {
                self->u_hweights[kh] += dnet * self->outputs[j];
                kh++;
            }
            self->u_biases[i] += dnet;
            k++;
        }
    }

#if DEBUG_LAYER
    fprintf(stderr, "RNNLayer_feedBack(Layer%d):\n", self->lid);
    for (int i = 0; i < self->nnodes; i++) {
        double y = self->outputs[i];
        double g = tanh_g(y);
        double dnet = self->errors[i] * g;
        fprintf(stderr, "  dnet = %.4f, dw = [", dnet);
        for (int j = 0; j < lprev->nnodes; j++) {
            double dw = dnet * lprev->outputs[j];
            fprintf(stderr, " %.4f", dw);
        }
        fprintf(stderr, "]\n");
    }
#endif
}


/* RNNLayer_setInputs(self, values)
   Sets the input values.
*/
void RNNLayer_setInputs(RNNLayer* self, const double* values)
{
    assert (self != NULL);
    assert (self->lprev == NULL);

#if DEBUG_LAYER
    fprintf(stderr, "RNNLayer_setInputs(Layer%d): values = [", self->lid);
    for (int i = 0; i < self->nnodes; i++) {
        fprintf(stderr, " %.4f", values[i]);
    }
    fprintf(stderr, "]\n");
#endif

    /* Set the values as the outputs. */
    for (int i = 0; i < self->nnodes; i++) {
        self->outputs[i] = values[i];
    }

    /* Start feed forwarding. */
    RNNLayer* layer = self->lnext;
    while (layer != NULL) {
        RNNLayer_feedForw(layer);
        layer = layer->lnext;
    }
}

/* RNNLayer_getOutputs(self, outputs)
   Gets the output values.
*/
void RNNLayer_getOutputs(const RNNLayer* self, double* outputs)
{
    assert (self != NULL);
    for (int i = 0; i < self->nnodes; i++) {
        outputs[i] = self->outputs[i];
    }
}

/* RNNLayer_getErrorTotal(self)
   Gets the error total.
*/
double RNNLayer_getErrorTotal(const RNNLayer* self)
{
    assert (self != NULL);
    double total = 0;
    for (int i = 0; i < self->nnodes; i++) {
        double e = self->errors[i];
        total += e*e;
    }
    return (total / self->nnodes);
}

/* RNNLayer_learnOutputs(self, values)
   Learns the output values.
*/
void RNNLayer_learnOutputs(RNNLayer* self, const double* values)
{
    assert (self != NULL);
    assert (self->lprev != NULL);
    for (int i = 0; i < self->nnodes; i++) {
        self->errors[i] = (self->outputs[i] - values[i]);
    }

#if DEBUG_LAYER
    fprintf(stderr, "RNNLayer_learnOutputs(Layer%d): errors = [", self->lid);
    for (int i = 0; i < self->nnodes; i++) {
        fprintf(stderr, " %.4f", self->errors[i]);
    }
    fprintf(stderr, "]\n");
#endif

    /* Start backpropagation. */
    RNNLayer* layer = self->lprev;
    while (layer != NULL) {
        RNNLayer_feedBack(layer);
        layer = layer->lprev;
    }
}

/* RNNLayer_update(self, rate)
   Updates the weights.
*/
void RNNLayer_update(RNNLayer* self, double rate)
{
#if DEBUG_LAYER
    fprintf(stderr, "RNNLayer_update(Layer%d): rate = %.4f\n", self->lid, rate);
#endif

    /* Update the bias and weights. */
    if (self->biases != NULL) {
        for (int i = 0; i < self->nbiases; i++) {
            self->biases[i] -= rate * self->u_biases[i];
            self->u_biases[i] = 0;
        }
    }
    if (self->xweights != NULL) {
        for (int i = 0; i < self->nxweights; i++) {
            self->xweights[i] -= rate * self->u_xweights[i];
            self->u_xweights[i] = 0;
        }
    }
    if (self->hweights != NULL) {
        for (int i = 0; i < self->nhweights; i++) {
            self->hweights[i] -= rate * self->u_hweights[i];
            self->u_hweights[i] = 0;
        }
    }

    /* Update the previous layer. */
    if (self->lprev != NULL) {
        RNNLayer_update(self->lprev, rate);
    }
}


/* main */
int main(int argc, char* argv[])
{
    int ntimes = 5;

    /* Use a fixed random seed for debugging. */
    srand(0);
    /* Initialize layers. */
    RNNLayer* linput = RNNLayer_create(NULL, 10, ntimes);
    RNNLayer* lhidden = RNNLayer_create(linput, 3, ntimes);
    RNNLayer* loutput = RNNLayer_create(lhidden, 1, ntimes);
    RNNLayer_dump(linput, stderr);
    RNNLayer_dump(lhidden, stderr);
    RNNLayer_dump(loutput, stderr);

    /* Run the network. */
    double rate = 1.0;
    int nepochs = 1000;
    for (int i = 0; i < nepochs; i++) {
        double x[10];
        double y[1];
        double t[1];
        RNNLayer_reset(linput);
        RNNLayer_reset(lhidden);
        RNNLayer_reset(loutput);
        for (int j = 0; j < ntimes; j++) {
            int p = f(i+j);
            for (int k = 0; k < 10; k++) {
                x[k] = (k == p)? 1 : 0;
            }
            t[0] = g(i+j);   /* answer */
            RNNLayer_setInputs(linput, x);
            RNNLayer_getOutputs(loutput, y);
            RNNLayer_learnOutputs(loutput, t);
            double etotal = RNNLayer_getErrorTotal(loutput);
            fprintf(stderr, "i=%d, x=%d, y=[%.4f], t=[%.4f], etotal=%.4f\n",
                    i, p, y[0], t[0], etotal);
        }
        RNNLayer_update(loutput, rate);
    }

    /* Dump the finished network. */
    RNNLayer_dump(linput, stdout);
    RNNLayer_dump(lhidden, stdout);
    RNNLayer_dump(loutput, stdout);

    RNNLayer_destroy(linput);
    RNNLayer_destroy(lhidden);
    RNNLayer_destroy(loutput);
    return 0;
}
