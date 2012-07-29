=head1 NAME

Bi::Visitor::InitialToParamTransformer - visitor for augmenting the
parameters of a model with its initial conditions.

=head1 SYNOPSIS

    use Bi::Visitor::InitialToParamTransformer;
    Bi::Visitor::InitialToParamTransformer->evaluate($model);

=head1 INHERITS

L<Bi::Visitor>

=cut

package Bi::Visitor::InitialToParamTransformer;

use base 'Bi::Visitor';
use warnings;
use strict;

use Carp::Assert;
use FindBin qw($Bin);
use File::Spec;

use Bi::Visitor::TargetReplacer;

=head1 METHODS

=over 4

=item B<evaluate>(I<model>)

Construct and evaluate.

=over 4

=item I<model> L<Bi::Model> object.

=back

No return value.

=cut
sub evaluate {
    my $class = shift;
    my $model = shift;

    my $self = {};
    bless $self, $class;

    my $initial_block = $model->get_block('initial')->clone($model);
    $initial_block->set_name('eval');
    $initial_block->set_commit(1);
    
    my $proposal_initial_block = $model->get_block('proposal_initial')->clone($model);
    $proposal_initial_block->set_name('eval');
    $proposal_initial_block->set_commit(1);
        
    # create params to hold initial conditions
    my $state;
    my $param;
    foreach $state (@{$model->get_vars('state')}) {
        my @dims = @{$state->get_dims};
        my @args = @{$state->get_args};
        my %named_args = %{$state->get_named_args};
        $named_args{'io'} = new Bi::Expression::Literal(0);
        
        $param = new Bi::Model::Param($state->get_name . "_0_", \@dims,
            \@args, \%named_args);
        $model->add_var($param);
        
        Bi::Visitor::TargetReplacer->evaluate($initial_block, $state, $param);
        Bi::Visitor::TargetReplacer->evaluate($proposal_initial_block, $state, $param);
    }
    
    $model->accept($self, $model, $initial_block, $proposal_initial_block);
}

=item B<visit>(I<node>)

Visit node of model.

=cut
sub visit {
    my $self = shift;
    my $node = shift;
    my $model = shift;
    my $initial_block = shift;
    my $proposal_initial_block = shift;

    if ($node->isa('Bi::Model::Block')) {
        if ($node->get_name eq 'parameter') {
            $node->sink_actions($model);
            $node->get_block($node->num_blocks - 1)->set_commit(1);
    		$node->push_block($initial_block);
        } elsif ($node->get_name eq 'proposal_parameter') {
            $node->sink_actions($model);
            $node->get_block($node->num_blocks - 1)->set_commit(1);
    		$node->push_block($proposal_initial_block);
        } elsif ($node->get_name eq 'initial' || $node->get_name eq 'proposal_initial') {
            # replace with dirac functions
            my $state;
            my $param;
            my $action;
            
            $node->clear;            
            foreach $state (@{$model->get_vars('state')}) {
                $param = $model->get_var($state->get_name . '_0_');
                $action = new Bi::Model::Action($model->next_action_id,
                    new Bi::Expression::VarIdentifier($state), '<-',
                    new Bi::Expression::VarIdentifier($param));
                $node->push_action($action);
            }
            
        }
    }
    return $node;
}

1;

=back

=head1 AUTHOR

Lawrence Murray <lawrence.murray@csiro.au>

=head1 VERSION

$Rev$ $Date$
