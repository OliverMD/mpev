import plotly as py
import plotly.graph_objs as go
import csv
import argparse
import collections

parser = argparse.ArgumentParser(description='Visualise Evolutionary Computation data')

parser.add_argument('src', type=str, help='csv file to parse')
parser.add_argument('--measure', type=str, default='mean', help='what to graph')
parser.add_argument('--output', type=str, default='results.html', help='name of output file')

if __name__ == "__main__":

    args = parser.parse_args()
    with open(args.src, 'rb') as d:
        gens = collections.defaultdict(list)
        vals = collections.defaultdict(list)

        rdr = csv.DictReader(d)
        for row in rdr:

            gens[int(row['pop'])].append(float(row['gen']))
            vals[int(row['pop'])].append(float(row[args.measure]))

    traces = []
    for k, v in gens.iteritems():
        traces.append(go.Scatter(x = v, y = vals[k]))

    py.offline.plot(traces, filename='results.html')