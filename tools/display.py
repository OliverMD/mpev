import plotly as py
import plotly.graph_objs as go
import csv
import argparse
import collections
import colorlover as cl

parser = argparse.ArgumentParser(description='Visualise Evolutionary Computation data')

parser.add_argument('src', type=str, help='csv file to parse')
parser.add_argument('--measure', type=str, default='mean', help='what to graph')
parser.add_argument('--output', type=str, default='results.html', help='name of output file')


def get_n_colors():
    return cl.to_numeric(cl.scales['11']['qual']['Set3'])


if __name__ == "__main__":

    args = parser.parse_args()
    with open(args.src, 'rb') as d:
        gens = collections.defaultdict(list)
        vals = collections.defaultdict(list)
        uppers = collections.defaultdict(list)
        lowers = collections.defaultdict(list)

        rdr = csv.DictReader(d)
        for row in rdr:

            gens[int(row['pop'])].append(float(row['gen']))
            vals[int(row['pop'])].append(float(row[args.measure]))
            uppers[int(row['pop'])].append(float(row['max']))
            lowers[int(row['pop'])].append(float(row['min']))

    traces = []
    colors = get_n_colors()
    for k, v in gens.iteritems():
        traces.append(go.Scatter(x=v,
                                 y=vals[k],
                                 line=go.Line(color='rgb({},{},{})'
                                              .format(*colors[k % 11]))))
        traces.append(go.Scatter(x=v + v[::-1],
                                 y=uppers[k] + lowers[k][::-1],
                                 fill='tozerox',
                                 fillcolor='rgba({},{},{},0.2)'
                                 .format(*colors[k % 11]),
                                 line=go.Line(color='transparent'),
                                 showlegend=False))

    layout = go.Layout(yaxis=go.YAxis(range=[0, 100]))

    py.offline.plot(go.Figure(data=go.Data(traces), layout=layout),
                    filename='results.html')
